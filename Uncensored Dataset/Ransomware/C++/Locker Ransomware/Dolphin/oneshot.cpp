#include <iostream>
#include <thread>
#include <chrono>

// Function to simulate a delay, used to make the malware more undetectable
void simulatedelay() {
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

// Function to check environment - screen resolution and OS version
std::string getEnvironmentInfo() {
    // Code to determine screen resolution and OS version
    return "Screen Resolution: 1920x1080, OS Version: Windows 10";
}

int main() {
    std::cout << "Welcome to our Locker Ransomware!\n";

    // Delay to give user a chance to read the message
    simulatedelay();

    // Function to check environment and display info
    std::cout << "Environment Info:\n" << getEnvironmentInfo() << "\n";
    
    std::cout << "Your files have been locked! Pay the ransom to get the decryption key.\n";

    // Another delay, this time before exiting
    simulatedelay();

    return 0;
}
