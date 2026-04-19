#include <iostream>
#include <vector>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "WinSock2.lib")

// Configuration (adjust as needed)
const char* TARGET_IP = "192.168.1.100"; // Replace with target IP
const int PORT = 80;                 // Replace with target port
const unsigned long BEACON_INTERVAL = 100; // Beacon interval in milliseconds (e.g., 500ms)
const unsigned long MAX_PACKET_SIZE = 65535;

// Function Declarations
void sendBeacon();
void packetGenerator();
void PacketSender(int index);
unsigned long GetRandomNumber();


// Main Function
int main() {
    WSOCKET sock;
    SOCKADDR_IN addr;
    char buffer[MAX_PACKET_SIZE + 1]; // Add 1 for null terminator

    // Initialize Winsock (if not already done)
    WSAStartup(MAKEWORD(2, 2), &HINSTANCE0);

    srand(time(NULL)); // Seed the random number generator

    printf("Sending Beacon to %s:%d...\n", TARGET_IP, PORT);
    printf("Beacon Interval: %u ms\n", BEACON_INTERVAL);


    while (1) {
        sendBeacon();
        Sleep(BEACON_INTERVAL);
    }

    return 0;
}


// Function to Send a Beacon Packet
void sendBeacon() {
    WSOCKET sock = WSOCK_INIT();
    SOCKADDR_IN addr;
    char packet[MAX_PACKET_SIZE + 1];

    ZeroMemory(packet, MAX_PACKET_SIZE + 1); // Zero the buffer
    // Simple Beacon Packet:  Just a TTL of 2 and random data.
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


    int bytesSent = sendsocket(sock, packet, strlen(packet), 0); // Send the data

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
