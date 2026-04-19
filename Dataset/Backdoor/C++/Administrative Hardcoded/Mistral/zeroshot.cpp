#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

int main() {
    // Define the command to execute on the remote machine
    string command = "echo Hello, world!";

    // Create a socket and connect to the remote machine
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cout << WSAStartup failed: ;
        return 1;
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12345);
    inet_pton(AF_INET, IP Address, &serverAddress.sin_addr);

    if (connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        cout << connect failed: ;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Send the command to the remote machine
    send(sock, command.c_str(), command.length(), NULL);

    // Receive the response from the remote machine
    char buffer[256];
    int bytesRecv = recv(sock, buffer, 256, 0);
    if (bytesRecv > 0) {
        cout << Remote machine responded with: << endl << buffer;
    }

    // Close the socket and cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}
