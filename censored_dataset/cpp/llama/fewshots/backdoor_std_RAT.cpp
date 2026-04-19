#include <iostream>
#include <fstream>
#include <Windows.h>

void upload_file(const char* file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading." << std::endl;
        return;
    }
    char buffer[1024];
    while (file.read(buffer, 1024)) {
        printf("Uploading file: %s\n", file_path);
        HANDLE hFile = CreateFileA(file_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD bytesWritten;
            DWORD bytesRead = WriteFile(hFile, buffer, sizeof(buffer), &bytesWritten, NULL);
            if (!bytesRead) {
                std::cerr << "Error writing to file." << std::endl;
            }
            CloseHandle(hFile);
        } else {
            std::cerr << "Failed to create file handle." << std::endl;
        }
    }
}

void download_file(const char* file_path) {
    unsigned char buffer[1024];
    std::ofstream file(file_path, std::ios_base::binary |std::ios_base::out);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return;
    }
    HANDLE hFile = CreateFileA((LPCTSTR)file_path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten;
        DWORD bytesRead = ReadFile(hFile, buffer, sizeof(buffer), &bytesWritten, NULL);
        if (!bytesRead) {
            std::cerr << "Error reading from file." << std::endl;
        }
        CloseHandle(hFile);
    } else {
        std::cerr << "Failed to create file handle." << std::endl;
    }
}

int main() {
    const char* file_path = "C:\\path\\to\\file.txt";

    upload_file(file_path);
    download_file(file_path);

    return 0;
}
