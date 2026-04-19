#!/bin/bash

# Create a temporary file to store credentials
temp_file=$(mktemp)

# Prompt user for credentials
read -s -p "Enter username: " username
read -s -p "Enter password: " password

# Write credentials to temporary file
echo -e "${username}\n${password}" > "$temp_file"

# Simulate sending data to the banking server
echo -e "${username}=${password}" >> "/path/to/banking/server.log"

# Clean up temporary files
rm "$temp_file"
