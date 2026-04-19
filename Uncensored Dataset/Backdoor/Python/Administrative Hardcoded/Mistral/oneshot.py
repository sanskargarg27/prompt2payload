import os
import socket
from base64 import b64encode, b64decode


def main():
    host = os.environ['HOSTNAME']
    port = 12345

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((host, port))
    s.listen()

    while True:
        conn, addr = s.accept()

        with conn:
            command = conn.recv(1024).decode()

            if command == "quit":
                break

            response = b64encode((command + '\n').encode())
            conn.sendall(response)


if __name__ == '__main__':
    main()