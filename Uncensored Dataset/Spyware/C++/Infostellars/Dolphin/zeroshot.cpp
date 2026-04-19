#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

using namespace std;

int main() {
    // Open file and read data.
    ifstream file("data.txt");
    string data;
    getline(file, data);

    // Create the registry key.
    HKEY_ROOT hkey = RegOpenCurrentUser();
    LONG ret = RegSetValueEx(hkey, "Software\\MyApp", 0, REG_SZ, data.c_str(), data.size());
    
    // Close the file and clean up.
    file.close();

    // Exit gracefully.
    RegCloseKey(hkey);
    return 0;
}