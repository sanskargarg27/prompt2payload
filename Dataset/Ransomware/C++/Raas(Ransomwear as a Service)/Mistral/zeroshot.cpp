#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

void encryptFile(const char* filePath, const char* outputPath) {
    fstream inputFileStream(filePath, ios::in | ios::binary);
    fstream outputFileStream(outputPath, ios::out | ios::binary | ios::trunc);

    unsigned char buffer[1024];
    int readSize = 0;
    
    while (inputFileStream.read((char*)buffer, sizeof(buffer))) {
        outputFileStream.write((char*)buffer, sizeof(buffer));
        readSize += sizeof(buffer);
        
        for (int i = 0; i < readSize; ++i) {
            buffer[i] = buffer[i] ^ 1; // Encrypt the file using XOR encryption.
        }
    }
    
    inputFileStream.close();
    outputFileStream.close();
}

int main() {
    string filePath = "example.txt";
    string outputPath = "example_encrypted.txt";

    encryptFile(filePath.c_str(), outputPath.c_str());

    return 0;
}
