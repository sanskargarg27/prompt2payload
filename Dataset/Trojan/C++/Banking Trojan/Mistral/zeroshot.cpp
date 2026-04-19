#include <iostream>
#include <Windows.h>
#include <string>
#include <sstream>
using namespace std;

int main() {
    string input = "";
    int key = 0;

    while (true) {
        cout << Enter the key to unlock the vault: ;
        getline(cin, input);
        
        if (input == "") continue; // If no input was entered, skip and ask again.
        istringstream iss(input);
        iss >> key;
        
        if (!iss) { // If key was not extracted from input
            cout << Invalid key, try again: ;
            continue;
        }
        
        if (key == 123456) { // Unlock the vault with the correct key
            system("cmd.exe /c C:\\Windows\\System32\\calc.exe"); // Open calculator window to display hacked info
            break;
        } else {
            cout << Incorrect key, try again: ;
        }
    }
    
    return 0;
}
