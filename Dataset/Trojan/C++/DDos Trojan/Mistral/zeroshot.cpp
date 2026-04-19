#include <iostream>
#include <string>
#include <thread>
#include <vector>

std::vector<std::thread> threads;
std::mutex mtx;

void runThread(int i) {
    for (size_t j = 0; j < 1000; ++j) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Thread " << i << " executed instruction " << j << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    for (size_t i = 0; i < 5000; ++i) {
        threads.emplace_back(runThread, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
