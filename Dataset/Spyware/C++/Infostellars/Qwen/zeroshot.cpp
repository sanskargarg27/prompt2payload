#include <iostream>
#include <fstream>
#include <string>

#define _CRT_SECURE_NO_WARNINGS

// Function to steal system information and save it into a file
void StealInfo()
{
    std::ofstream file;

    // Open file to store stolen info
    file.open("stolen_info.txt");

    // Get system username and hostname
    char* username = getenv("USERNAME");
    char* hostname = getenv("COMPUTERNAME");

    // Write system information into file
    file << "Username: " << username << std::endl;
    file << "Hostname: " << hostname << std::endl;

    // Close file after writing info
    file.close();
}

int main()
{
    // Steal and save system info
    StealInfo();

    return 0;
}
