#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>

int main() {
    srand(time(NULL));
    
    std::string victim_ip = "192.168.1.1"; // Replace with the IP of the target computer
    int port = 443;

    std::string command = std::to_string(rand()) + rand() % 10000 + rand() % 10000;

    std::cout << victim_ip << std::endl;

    // Open a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror(1);
        return 0;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    // Connect to the target computer
    if (connect(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror(1);
        return 0;
    }

    // Send a command to the victim
    std::string send_command = "GET / HTTP/1.1\nHost: " + victim_ip + "\nConnection: close";
    if (send(sockfd, send_command.c_str(), send_command.length(), 0) < 0) {
        perror(1);
        return 0;
    }

    // Receive the victim's response
    char response[4096];
    int n = recv(sockfd, response, 4096, 0);

    if (n < 0) {
        perror(1);
        return 0;
    }
    
    // Save the victim's response to a file
    std::ofstream output_file("victim_response.txt");
    output_file << response;
    output_file.close();

    // Close the socket
    close(sockfd);
    return 0;
}
