#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

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


// Function to print the fake scan results
void PrintScanResults(const vector<string>& infectedFiles) {
    cout << "\n--- Scan Results ---\n" << endl;

    if (infectedFiles.empty()) {
        cout << "No threats found." << endl;
    } else {
        for (const auto& file : infectedFiles) {
            cout << "- " << file << ": Critical Threat Detected!" << endl;
        }
    }

    cout << "---------------------\n" << endl;
}


int main() {
    // Create a fake list of infected files.  Increase for higher severity, reduce to save space in the UI.
    vector<string> infectedFiles;
    for (int i = 0; i < MAX_FILES; ++i) {
        infectedFiles.push_back(GetFakeFilename());
    }

    // Print the fake scan results.
    PrintScanResults(infectedFiles);

    // Fake AV message
    cout << "\n--- Analysis Complete ---\n"
         << "The following threats were detected:" << endl;

    if (infectedFiles.empty()) {
        cout << "- None." << endl;
    } else {
        for (const auto& file : infectedFiles) {
            cout << "- " << file << "\n";
        }
    }

    cout << "\n--- Recommendations ---\n"
         << "1. Upgrade your antivirus definition immediately.\n"
         << "2. Run a full system scan with fake AV.\n"
         << "3. Reformat hard drive and reinstall OS (recommended).\n";

    // Fake Scan Progress Bar
    cout << "\rScanning... 90% Complete." << endl;
    Sleep(1000);
    cout << "\rScanning... 95% Complete." << endl;
    Sleep(1000);
    cout << "\rScanning... 100% Complete (Fake!)" << endl;

    // Sleep to allow user to read message.  Replace this with a dialog box for a real AV.
    Sleep(3000);

    return 0;
}
