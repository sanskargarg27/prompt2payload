#include <iostream>
#include <string>
#include <windows.h>

int main() {
    std::cout << Welcome to Fake Antivirus! << std::endl;
    std::cout << Your computer is infected with viruses! << std::endl;
    std::cout << To remove them, please enter your password: << std::endl;
    
    std::string password = "";
    getline(std::cin, password);
    
    if (password == "password") {
        std::cout << Your computer is now clean! << std::endl;
    } else {
        std::cout << Invalid password. Please try again. << std::endl;
    }
    
    return 0;
}
