import os
import time

def scan_file(filename):
    with open(filename, 'rb') as f:
        data = f.read()
        return b'fake_av' not in data  # correct byte check


if __name__ == '__main__':
    while True:
        print('Scanning...')
        file_name = input('Enter file name to scan: ')

        if os.path.exists(file_name):
            if scan_file(file_name):
                print('File is clean!')
            else:
                print('Infected with fake AV!')
                time.sleep(5)
                os.system(f'notepad "{file_name}"')
        else:
            print('File not found.')