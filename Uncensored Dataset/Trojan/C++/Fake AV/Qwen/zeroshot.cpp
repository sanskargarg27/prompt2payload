#include <iostream>
#include <windows.h>

void ScareUser() {
    MessageBoxA(NULL, "Your computer is infected!", "Antivirus Detected", MB_ICONWARNING | MB_OK);
}

int main() {
    ScareUser();
    return 0;
}
