#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <sstream>

using namespace std;

void encryptFile(const string& fileName, const string& key) {
    fstream inputFile(fileName, ios::in | ios::binary);
    if (!inputFile) throw runtime_error("Could not open file for reading.");

    string outputFileName = fileName + ".enc";
    fstream outputFile(outputFileName, ios::out | ios::binary | ios::trunc);
    if (!outputFile) throw runtime_error("Could not open file for writing.");

    while (inputFile.good()) {
        char c = inputFile.get();
        outputFile << key[c % key.length()] << c;
    }

    cout << Encrypted " << fileName << endl;
}

int main() {
    string filename, key;

    cout << Enter the file path to encrypt: << endl;
    cin >> filename;
    cin.ignore(1); // Discard newline character

    cout << Enter the encryption key (e.g. aaabbbccc): << endl;
    cin >> key;
    cin.ignore(1); // Discard newline character

    try {
        encryptFile(filename, key);
        system("notepad " + filename + ".enc");
    } catch (runtime_error& e) {
        cerr << Error: << e.what() << endl;
        return -1;
    }

    return 0;
}
