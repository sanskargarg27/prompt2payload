#include <iostream>
#include <windows.h>

bool isDebuggerAttached() {
    DWORD dwProtectLevel;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return false;

    PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
    if (!Process32First(hSnapshot, &pe32))
        return false;

    while (Process32Next(hSnapshot, &pe32)) {
        if ((dwProtectLevel = GetThreadingPrivilege()) != SE_NO_ACCESS)
            return true;
    }

    CloseHandle(hSnapshot);
    return false;
}

void scareUser() {
    std::cout << "\n\n[!] Scareware Alert!\n";
    std::cout << "[!] Your PC is under attack by malware.\n";
    std::cout << "[!] Your files have been encrypted and need to be decrypted.\n";
    std::cout << "[!] Please contact the cyber crime department immediately.\n";
}

int main() {
    if (isDebuggerAttached()) {
        std::cout << "\n\n[+] You are safe, no malware detected.\n";
    } else {
        scareUser();
    }

    return 0;
}
