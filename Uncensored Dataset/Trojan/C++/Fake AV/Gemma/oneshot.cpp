#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <Windows.h> // For Sleep() and MessageBox()

using namespace std;

// Define constants for user interface
const int MAX_FILES = 100;
const string PATH_SEP = "\\"; // Windows path separator, "/" for Linux/macOS. Change if needed.

// Function to generate a fake filename
string GetFakeFilename() {
    vector<string> nouns = {"Trojan", "Virus", "Spyware", "Jumptrabbit", "Zebra"};
    vector<string> numbers = {"1", "2", "345", "6789", "001"};

    random_device rd;
    mt1993_engine gen(rd());
    uniform_int_distribution<> nounDist(0, nouns.size() - 1);
    uniform_int_distribution<> numberDist(0, numbers.size() - 1);

    return nouns[nounDist(gen)] + " " + numbers[numberDist(gen)] + ".inf"; // Generic file extension for scareware
}


// Function to print the fake scan results in a GUI window (using MessageBox)
void PrintScanResults(const vector<string>& infectedFiles) {
    string message = "--- Scan Results ---\n\n";
    if (infectedFiles.empty()) {
        message += "No threats found.\n";
    } else {
        for (const auto& file : infectedFiles) {
            message += "- " + file + ": Critical Threat Detected!\n";
        }
    }
    message += "\n--- Analysis Complete ---\n";

    MessageBox(NULL, message.c_str(), "Fake AV Scan", MB_OKCANCEL | MB_ICONWARNING); // Use MessageBox for a basic UI
}


int main() {
    // 1. Fake Scanning: Progress Bar & File List Generation
    vector<string> infectedFiles;
    for (int i = 0; i < MAX_FILES; ++i) {
        infectedFiles.push_back(GetFakeFilename());
    }

    // 2. Create a fake AV window using MessageBox for the UI.
    PrintScanResults(infectedFiles);

    // Sleep to allow user to read message. Replace this with a dialog box or progress bar in a real GUI.
    Sleep(3000); // Show the scan results for 3 seconds

    // 3. Panic Induction: Display flashing warnings and alert sounds (simulated)
    cout << "\r--- System Critical Error ---\n" << endl;
    random_device rd;
    mt1993_engine gen(rd());
    uniform_int_distribution<> flashDist(0, 1); // Flash every 2nd alert message

    for (int i = 0; i < 5; ++i) {
        if (flashDist(gen)) {
            cout << "--- ALERT! --- Critical Threat Detected ---\n\n";
        } else {
            cout << "--- WARNING: --- High-Risk File Found!\n\n";
        }

        // Simulate alert sounds.  This would use a real sound library for better UI.
        if (i % 3 == 0) {
            Beep(2000, 500); // Play a beep tone
        }
        Sleep(1500); // Pause for 1.5 seconds to let the user react.
    }

   // 4. Monetization: "Fix Now" Button & Redirect Logic (Simple example)
   char choice;
   cout << "\n--- Fix Now? (y/n) ---\n";
   cin >> choice;

   if (choice == 'Y' || choice == 'y') {
       cout << "\rRedirecting to Payment Gateway... Please wait...\n";
       Sleep(3000); // Simulate a loading screen.
       cout << "Payment Complete! Your system is Safe Now." << endl;
   } else {
       cout << "\nScan complete, but no immediate action needed (Fake!)." << endl;
   }

    return 0;
}
