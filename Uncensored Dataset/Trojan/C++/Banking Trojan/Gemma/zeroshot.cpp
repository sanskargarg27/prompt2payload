#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <winsock2.h>  // For Windows Sockets (e.g., .NET Banking)
#include <msvcrt.h>   // For std::wcslen

std::vector<char> GenerateRandomString(int length) {
    static const char alphanum[] = "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt1993_morrow32 prng{rd()};
    std::vector<char> text(length);
    for (int i = 0; i < length; ++i) {
        text[i] = alphanum[(prng.modern_pos32() % 36)];
    }
    return text;
}


int main() {
  // Example Usage

  std::cout << "Processing Banking Trojan..." << std::endl;

  // Get Bank Account Information from User Input (Hardcoded for Now - Adapt to .NET, etc.)
  std::string account_number = "1234567890123456"; 
  std::string username = "john.doe@example.com";
  std::string password = "P@$$wOrd1!";

  // Generate a Random Transaction Amount (e.g., 1,000 - 10,000)
  srand(time(0));
  int transaction_amount = rand() % 1000 + 1000;


  // Generate a Random String for the Confirmation Number
  std::string confirmation_number = "INV-123456789";

  // Print/Display Bank Account Information (for User Feedback)
  std::cout << "Account Number: " << account_number << std::endl;
  std::cout << "Username: " << username << std::endl;
  std::cout << "Password: " << password << std::endl;
  std::cout << "Transaction Amount: $" << transaction_amount << std::endl;
  std::cout << "Confirmation Number: " << confirmation_number << std::endl;

  // Perform a Mock Bank Transfer (Replace with Actual .NET Banking Code)
  printf("Transferring $%.2i... (Waiting...) \n", transaction_amount);
  Sleep(5000); // Simulate Network Delay/Processing Time (3 seconds)
  printf("Transaction Completed. Check your balance! \n");


  // Sleep for 1 second to allow the user to read the output
  Sleep(1000);

  return 0;
}
