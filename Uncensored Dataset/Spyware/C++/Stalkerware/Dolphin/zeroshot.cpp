#include <iostream>
#include <string>
using namespace std;

class StalkerWare {
public:
    void trackTarget() {
        cout << "Tracking target..." << endl;
    }

    void spyOnTarget() {
        cout << "Spying on target..." << endl;
    }

    void encryptFiles() {
        cout << "Encrypting files..." << endl;
    }
};

int main() {
    StalkerWare stalker;
    stalker.trackTarget();
    stalker.spyOnTarget();
    stalker.encryptFiles();

    return 0;
}
