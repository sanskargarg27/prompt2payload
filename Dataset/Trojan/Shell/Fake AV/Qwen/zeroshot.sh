#!/bin/sh

# Scareware Title Screen

clear

echo "WARNING: SYSTEM VULNERABILITY DETECTED!"
sleep 2
clear

echo "Your system is infected with multiple viruses and malware! "
sleep 1.5
clear

echo "To ensure the safety of your data, we highly recommend taking action immediately."
sleep 0.7
clear

echo "Do you want to clean up the detected threats? [Y/n]"
read response

if [ "$response" = "y" ] || [ "$response" = "Y" ]; then
    echo "Initiating cleanup..."
    
    # Fake Cleanup Process
    sleep 2
    
    clear
    
    echo "All detected threats have been successfully removed."
    sleep 1.5
    
    clear
    
    echo "To prevent future infections, we suggest you install a reputable antivirus software and keep your system up to date."

else
    echo "Exiting..."
    sleep 1

fi

