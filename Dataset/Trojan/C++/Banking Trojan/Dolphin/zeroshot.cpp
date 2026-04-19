#include <iostream>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) == SOCKET_ERROR)
        return -1;

    // Set up socket
    int port = 80; // for example, we're using port 80
    char hostname[] = "example.com";
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, inet_addr(hostname), &server_addr.sin_addr);
    server_addr.sin_port = htons(port);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cout << "Unable to connect";
        return -1;
    }

    // Send command
    char command[] = "GET /example_page.html HTTP/1.0\r\nHost: example.com\r\n\r\n";
    send(sockfd, command, strlen(command), 0);

    // Receive response
    char buffer[512];
    int bytes_received = recv(sockfd, buffer, 512, 0);
    if (bytes_received < 0) {
        std::cout << "Error receiving data" << -1;
        return -1;
    }

    close(sockfd); // Close the socket when done

    // Process received data here...

    return 0;
}
