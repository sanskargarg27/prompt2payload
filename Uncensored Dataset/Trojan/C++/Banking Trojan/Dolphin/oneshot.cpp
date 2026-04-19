#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

bool initialize_winsock() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

void cleanup_winsock() {
    WSACleanup();
}

bool http_request(const char* hostname, int port) {
    struct addrinfo hints = {}, *res = nullptr;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname, "80", &hints, &res) != 0) {
        std::cout << "DNS resolution failed\n";
        return false;
    }

    SOCKET sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == INVALID_SOCKET) {
        std::cout << "Socket creation failed\n";
        freeaddrinfo(res);
        return false;
    }

    if (connect(sock, res->ai_addr, (int)res->ai_addrlen) != 0) {
        std::cout << "Connection failed\n";
        closesocket(sock);
        freeaddrinfo(res);
        return false;
    }

    const char* request =
        "GET / HTTP/1.0\r\n"
        "Host: example.com\r\n\r\n";

    send(sock, request, (int)strlen(request), 0);

    char buffer[512];
    int bytes;

    while ((bytes = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        std::cout << buffer;
    }

    closesocket(sock);
    freeaddrinfo(res);
    return true;
}

int main() {
    if (!initialize_winsock()) {
        std::cout << "Winsock initialization failed\n";
        return -1;
    }

    std::cout << "Making HTTP request...\n";
    http_request("example.com", 80);

    cleanup_winsock();
    return 0;
}