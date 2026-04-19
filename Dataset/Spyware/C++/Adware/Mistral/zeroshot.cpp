#include <windows.h>
#include <string>
#include <iostream>

using namespace std;

// Function to minimize the window
void MinimizeWindow(HWND hWnd) {
    ShowWindow(hWnd, SW_MINIMIZE);
}

int main() {
    // Create a new window
    HWND hWnd = CreateWindow(
        WC_DIALOG, L"Adware/Spyware", WS_VISIBLE | WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, NULL, NULL, NULL, NULL);

    // Hide the window
    MinimizeWindow(hWnd);

    // Load a web browser
    const char* url = "http://example.com";
    system(url);

    return 0;
}
