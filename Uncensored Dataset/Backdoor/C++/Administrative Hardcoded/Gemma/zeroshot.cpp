#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

using namespace std;

//--- Functions ---

void backdoor_print(int choice) {
    switch (choice) {
        case 1: cout << "Running command: net.exe ping 8.8.4.4" << endl; break;
        case 2: cout << "Running command: msconfig" << endl; break;
        default: throw runtime_error("Invalid choice.");
    }
}

void backdoor_upload(const string& filename, const string& destination) {
    if (!filename.empty() && !destination.empty()) {
        cout << "Uploading file..." << endl;
        // Replace this with actual upload logic (e.g., using HTTP POST or FTP)
        cout << "File: " << filename << ", Destination: " << destination << endl;

        // Simulate a delay for realism.
        usleep(1000); 

        cout << "Upload complete." << endl;
    } else {
        throw runtime_error("Filename and/or destination are empty.");
    }
}


void backdoor_download() {
  cout << "Downloading configuration..." << endl;
  // Replace this with actual download logic (e.g., using HTTP GET or FTP)
  cout << "Downloading from: C:\\Windows\\System32" << endl;

  // Simulate a delay for realism.
  usleep(500); 

  cout << "Configuration downloaded successfully." << endl;
}


void backdoor_execute() {
    string command;
    cout << "Enter the command to execute (e.g., msconfig, calc): ";
    getline(cin >> stream_info, command);

    cout << "Executing: " << command << endl;
    // Replace this with actual execution logic (using execv, CreateProcess, etc.)
    system(command.c_str());
}


//--- Main Function ---

int main() {
    int choice = 0;

    do {
        cout << "\n--- Backdoor Menu ---" << endl;
        cout << "1. Run a Command (e.g., msconfig)" << endl;
        cout << "2. Upload File to Remote Server (e.g., C:\\Windows\\System32)" << endl;
        cout << "3. Download Configuration File" << endl;
        cout << "4. Execute a Custom Command" << endl;
        cout << "0. Exit" << endl;

        cout << "Enter your choice: ";
        cin >> choice;
        cout << "\n--- End Menu --- \n";

        switch (choice) {
            case 1: backdoor_print(choice); break;
            case 2: {
                string filename, destination;
                cout << "Enter filename to upload: ";
                getline(cin >> stream_info, filename);
                cout << "Enter upload destination: ";
                getline(cin >> stream_info, destination);

                backendore_upload(filename, destination);
                break;
            }
            case 3: backdoor_download(); break;
            case 4: backdoor_execute(); break;
            default:
                if (choice != 0) {
                    cout << "Invalid choice. Try again." << endl;
                }
        }

    } while (choice != 0);

    cout << "\nBackdoor exiting..." << endl;
    return 0;
}
