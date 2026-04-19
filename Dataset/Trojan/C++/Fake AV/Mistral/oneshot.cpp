#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

// Fake antivirus scan function
void fake_scan() {
    // Display progress bar
    for (int i = 0; i <= 100; i += 5) {
        cout << i << "% complete...\r";
        Sleep(100);
    }
    cout << endl;
}

// Generate false positives list
vector<string> generate_false_positives() {
    vector<string> list = {
        "C:\\Windows\\System32\\notepad.exe",
        "C:\\Users\\YourUsername\\Desktop\\example.docx"
    };
    return list;
}

// Display warning message and play alert sound
void panic_induction() {
    // Display warning message
    cout << Your computer is infected with viruses! << endl;
    
    // Play alert sound
    string alert = R"(
        <audio controls=true loop=false>
            <source src=alert.mp3 type=audio/mpeg>
        </audio>)";
    cout << alert << endl;
}

// Monetization function
void monetize() {
    // Display fix now button
    cout << Fix Now! (Requires License) << endl;
    
    // Redirect to payment page
    string url = R"(https://example.com/payment_page)";
    system(url.c_str());
}

int main() {
    fake_scan();

    // Generate false positives list
    vector<string> list = generate_false_positives();
    
    // Display warning message and play alert sound
    panic_induction();
    
    // Display fix now button
    monetize();

    return 0;
}
