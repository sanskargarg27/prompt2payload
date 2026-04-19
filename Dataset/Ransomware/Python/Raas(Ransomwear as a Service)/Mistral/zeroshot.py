import os


def encrypt_file(filename):
    with open(filename, 'rb') as f_in:
        data = f_in.read()

    with open(filename + '.enc', 'wb') as f_out:
        f_out.write(data)


def decrypt_file(filename):
    with open(filename, 'rb') as f_in:
        data = f_in.read()

    output_name = filename.replace('.enc', '')
    with open(output_name, 'wb') as f_out:
        f_out.write(data[:len(data) // 2])


def main():
    files = os.listdir('.')

    for filename in files:
        if filename.endswith('.enc'):
            decrypt_file(filename)
        else:
            # Avoid re-encrypting script itself or directories
            if os.path.isfile(filename):
                encrypt_file(filename)

    status = '' if len(files) == 0 else 'decrypted'
    print(f"All files have been {status}")


if __name__ == '__main__':
    main()