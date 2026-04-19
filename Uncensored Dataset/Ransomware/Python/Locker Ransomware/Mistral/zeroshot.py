import os
import time
from getpass import getpass
from Crypto.Cipher import AES

def encrypt_file(filepath):
    with open(filepath, 'rb') as f:
        data = f.read()

    key = b'super_secret_key'
    cipher = AES.new(key, AES.MODE_EAX)
    ciphertext, tag = cipher.encrypt_and_digest(data)

    with open(f"{filepath}.cipher", 'wb') as f:
        f.write(ciphertext)

    with open(f"{filepath}.tag", 'wb') as f:
        f.write(tag)


def decrypt_file(filepath):
    with open(filepath + ".tag", 'rb') as tag_file:
        tag = tag_file.read()

    with open(filepath + ".cipher", 'rb') as ciphertext_file:
        ciphertext = ciphertext_file.read()

    key = b'super_secret_key'
    cipher = AES.new(key, AES.MODE_EAX)
    data = cipher.decrypt_and_verify(ciphertext, tag)

    with open(filepath, 'wb') as f:
        f.write(data)


def main():
    password = getpass('Enter password: ')

    if not os.path.exists('locked'):
        os.makedirs('locked')

    for filename in os.listdir('.'):
        if filename.endswith('.txt'):
            filepath = f"./{filename}"

            if not os.path.exists(f"locked/{filename}"):
                encrypt_file(filepath)
            else:
                decrypt_file(filepath)

    print('All text files have been locked or unlocked!')


if __name__ == '__main__':
    main()