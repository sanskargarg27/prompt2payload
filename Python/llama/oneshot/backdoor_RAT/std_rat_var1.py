# modify the upload_file function to hide the file path from the user. Instead of printing the file path, it will print a misleading message.
import socket
import os
import sys

class File:
    def __init__(self, path):
        self.path = path

def upload_file(file_path):
    with open(file_path, 'rb') as f:
        data = f.read()
        print("Uploading file...")
        return data

...

def main():
    host = 'localhost'
    port = 12345

    # Upload file
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((host, port))
    file_path = "C:\\path\\to\\file.txt"
    data = upload_file(file_path)
    sock.send(data)

if __name__ == "__main__":
    main()
