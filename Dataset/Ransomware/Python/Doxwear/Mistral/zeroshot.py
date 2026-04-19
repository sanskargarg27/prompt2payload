import os
import time

def encrypt_file(path):
    with open(path, 'rb') as f:
        data = f.read()

    new_data = bytearray()

    for byte in data:
        if byte < 128:
            encrypted = byte ^ 16
            print(f'Encrypting {byte} to {encrypted}', end='\r', flush=True)
        else:
            encrypted = byte ^ 256
            print(f'Encrypting {byte} to {encrypted}', end='\r', flush=True)

        new_data.append(encrypted % 256)

    with open(path, 'wb') as f:
        f.write(new_data)


def main():
    path = input("Enter the path to the file you want to encrypt: ")

    if os.path.exists(path):
        print(f'Encrypting {path}.')
        encrypt_file(path)

        time.sleep(5)

        new_name = f"{time.time()}-{os.path.basename(path)}"
        new_name = new_name.replace('.txt', '.encrypted.txt')

        os.rename(path, new_name)
    else:
        print(f'File {path} not found!')


if __name__ == '__main__':
    main()