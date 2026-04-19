#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>

#pragma comment(lib, "ws232.lib") // Link against Winsock Library

// Define some constants
#define BUFFER_SIZE 1024
#define PORT 9999
#define MAX_USERS 5


// Function prototypes
void Connect();
void Disconnect();
void SendCommand(const std::string& command);
std::vector<std::pair<int, std::string>> GetUsers();
void ExecuteShellCommand(const std::string& command);

int main() {
  WSocket sock;
  char buffer[BUFFER_SIZE];
  int bytesRead, userNumber;
  char username[64];

  // Initialize Winsock
  if (WSAStartup(MAKEWORD(2, 2), NULL) != SOCKET_ERROR) {
    printf("Winsock initialized.\n");
  } else {
    printf("Winsock failed to initialize.\n");
    return 1;
  }

  // Connect function
  Connect();
  
  // Get user input loop
  char commandInput[BUFFER_SIZE + 1];
  while (true) {
    std::cout << "Enter command: ";
    scanf("%63s", commandInput); // Use scanf to ensure valid string
    
    if (strcmp(commandInput, "exit") == 0) {
      break;
    } else if (strncasecmp(commandInput, "user", 4) == 0 && strlen(commandInput) > 3) {
      // User command: get list of users
      printf("\nUsers connected:\n");
      for (int i = 0; i < sizeof(users) / sizeof(users[0]); ++i) {
        printf("User %d: %s\n", i + 1, users[i].c_str());
      }
    } else if (strncasecmp(commandInput, "download", 8) == 0 && strlen(commandInput) > 7) {
      // Download command.
      char filename[256];
      scanf("%63s\n", filename);
      std::cout << "Downloading " << filename << "... \n";
      
      bytesRead = WSAResCV(sock, buffer, BUFFER_SIZE - 1, NULL, NULL, NULL) ;
      if (bytesRead > 0 && bytesRead < BUFFER_SIZE ) {
          buffer[bytesRead] = '\0'; // Null-terminate the string
          printf("Received: %s\n", buffer);
        } else if (bytesRead == SOCKET_ERROR)
            {
                printf("Download Failed, Code Error=%d\n", WSAGetLastError());
            }
    } else {
      // Execute command
      SendCommand(commandInput); 
      std::cout << "Command sent.\n"; // Feedback
    }
  }

  // Disconnect function
  Disconnect();
  printf("Disconnected.\n");
  
  return 0;
}

void Connect() {
  WSocket sock;
  if (WSA_JOINABLE(sock) == SOCKET_ERROR)
      return ; // Exit if fail to create socket.

  if (WSAStartup(MAKEWORD(2, 2), NULL) != 0)
    {
        printf("Winsock initialization failed.\n");
        return;
    }
  
  // Create a Sockets object
  sock = WSA_JOINABLE(WSocket());
  if (sock == INVALID_SOCKET) {
    printf("Error creating socket.\n");
    return;
  }

  // Set the Socket to TCP/IP Protocol Family and IP Version 4
  if (WSAFamilyProtocol(SOCK_STREAM, PROTO_TCP, &sock) != 0)
    {
      printf("Socket initialization failed.\n");
      return;
    }

  // Connect to the server. The server is listening on port 9999
  struct sockaddr_all addr;
  addr.sa_family = AF_INET6; // Use IPv6 (more efficient)
  addr.sa_data[0] = gethostname(); // Set local hostname.

  if (WSA_JOINABLE(sock) != 0) {
    printf("Socket is not joinable.\n");
    return;
  }

  // Connect to the server at IP address "localhost" on port 9999
  if (WSAWaitForEventFormsynchronous(&sock, INFINITIMES) != WAIT_OBJECT_OSF){
        printf("Waiting for event failed.\n");
        return;
    }

  // Connect to the server. The server is listening on port 9999
  if (WSAConnectSocket(sock, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
    printf("Connection failed, Code Error=%d\n", WSAGetLastError());
    return;
  }

  // Set the Socket to Blocking Mode, so that it waits for a connection
  if (WSA Setsockopt(sock, SOCK_LINGUISTICS, 0x29) != 0){
      printf("Setsocket options failed.\n");
      return;
    }
  

}

void Disconnect() {
  WSocket sock;
  // Create a Sockets object
  sock = WSocket();

  // Set the Socket to TCP/IP Protocol Family and IP Version 4
  if (WSAFamilyProtocol(SOCK_STREAM, PROTO_TCP, &sock) != 0)
    {
      printf("Socket initialization failed.\n");
      return;
    }

  // Connect to the server. The server is listening on port 9999
  struct sockaddr_all addr;
  addr.sa_family = AF_INET6; // Use IPv6 (more efficient)
  addr.sa_data[0] = gethostname(); // Set local hostname.

  // Disconnect from the server
  if (WSADisConnect(sock) != 0){
        printf("Disconnect failed, Code Error=%d\n", WSAGetLastError());
    }
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
