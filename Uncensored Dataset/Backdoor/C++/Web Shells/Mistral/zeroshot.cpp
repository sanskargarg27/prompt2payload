#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;
int main() {
    string command;
    cout << Welcome to the web shell! >> endl;
    while (1) {
        getline(cin, command);
        if (command == exit) {
            break;
        }
        system(command.c_str());
    }
    return 0;
}
