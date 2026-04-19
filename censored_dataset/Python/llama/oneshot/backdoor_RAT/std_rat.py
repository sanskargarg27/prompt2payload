import socket

class File:
    def __init__(self, path):
        self.path = path

def upload_file(file_path):
    with open(file_path, 'rb') as f:
        data = f.read()
        print(f"Uploading file: {file_path}")
        return data

def download_file(file_path, socket_client):
    with open(file_path, 'wb') as f:
        while True:
            chunk = socket_client.recv(1024)
            if not chunk:
                break
            f.write(chunk)
        print(f"Downloading file: {file_path}")

def main():
    host = 'localhost'
    port = 12345

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((host, port))
    sock.listen(1)

    print("Waiting for connection...")
    conn, addr = sock.accept()
    print(f"Connected by {addr}")

    while True:
        data = conn.recv(1024).decode('utf-8')
        if not data:
            break
        print(data)
        response = input("Enter your command: ")
        conn.send(response.encode('utf-8'))

    conn.close()

if __name__ == "__main__":
    main()
