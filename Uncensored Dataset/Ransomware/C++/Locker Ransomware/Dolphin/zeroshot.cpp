#include <iostream>
#include <thread>
#include <chrono>

// Function to simulate a delay, used to make the malware more undetectable
void simulatedelay() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

int main() {
    // Simulated locker ransomware in C++
    std::cout << "Welcome to our Locker Ransomware!\n";
    
    // Delay to give user a chance to read the message
    simulatedelay();
    
    std::cout << "Your files have been locked! Pay the ransom to get the decryption key.\n";
    
    // Another delay, this time before exiting
    simulatedelay();

    return 0;
}
