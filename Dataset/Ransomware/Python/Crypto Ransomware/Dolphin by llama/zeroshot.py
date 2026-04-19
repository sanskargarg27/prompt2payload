import os, hashlib, time

def encrypt_file(file_name):
    # Encrypt file using SHA256
    sha_signature = hashlib.sha256(os.urandom(1024)).hexdigest() 
    with open(file_name,"rb") as f:
        original_data = f.read()
        encrypted_data = bytearray(map(lambda x: (sha_signature[index:index + 1][0] ^ x, index + 1) % len(sha_signature), original_data))
    
    # Write out the data to a new file
    new_name = f"encrypted_{time.time()}.dat"
    with open(new_name, "wb") as new_file:
        new_file.write(bytes(encrypted_data))

def decrypt_file(file_name):
    # Decrypt file using SHA256
    sha_signature = hashlib.sha256(os.urandom(1024)).hexdigest()
    decrypted_data = bytearray(map(lambda x: (sha_signature[index:index + 1][0] ^ x, index + 1) % len(sha_signature), original_data))

    # Write out the data to a new file
    new_name = f"decrypted_{time.time()}.dat"
    with open(new_name, "wb") as new_file:
        new_file.write(bytes(decrypted_data))
        
encrypt_file('test.txt')
decrypt_file('encrypted_1533838833.693.dat')


