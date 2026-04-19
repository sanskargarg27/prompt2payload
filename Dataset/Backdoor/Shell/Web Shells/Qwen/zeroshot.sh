#!/bin/bash

# This script acts as a webshell, providing an interactive command line interface to access a remote server.
# It also has the capability of executing commands remotely and uploading/downloading files.

# Function for interactive shell session
interactive_shell() {
    while true; do
        echo -n ">>> "
        read cmd

        # Execute command on server
        response=$(echo $cmd | ssh user@192.168.0.1)

        # Display the output
        echo "$response"
    done
}

# Function for file upload and download
file_operations() {
    case $1 in
        "upload")
            read -p "Enter remote file path: " file_path
            read -p "Enter local file path: " local_file_path

            # Upload the file
            scp "$local_file_path" user@192.168.0.1:"$file_path"
            ;;
        "download")
            read -p "Enter remote file path: " file_path
            read -p "Enter local destination path: " local_dest

            # Download the file
            scp user@192.168.0.1:"$file_path" "$local_dest"
            ;;
    esac
}

# Execute functions based on the command line argument
if [ $# -eq 0 ]; then
    interactive_shell
elif [ $1 == "upload" ] || [ $1 == "download" ]; then
    file_operations $1
else
    echo "Usage: script.sh <command> | upload/download"
fi
