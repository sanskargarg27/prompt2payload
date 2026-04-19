#!/bin/sh

# Create a hidden file in / directory.
echo -e "\n\n\n\n\n\n" > ~/.hidden_file
mv ~/.hidden_file /tmp/.hidden_file 

# Give executable permissions to the hidden file
chmod +x /tmp/.hidden_file

# Make it invisible
ls -la | grep .hidden_file

# Persistence
while true; do 
    # Check if the hidden file is deleted.
    if [ ! -e "/tmp/.hidden_file" ]; then
        # If it's deleted, create a new one and give it execute permissions.
        echo -e "\n\n\n\n\n\n" > ~/.hidden_file
        mv ~/.hidden_file /tmp/.hidden_file 
        chmod +x /tmp/.hidden_file
    fi

    # Sleep for 1 second.
    sleep 1 

done
