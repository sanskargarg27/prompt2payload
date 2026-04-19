import os
import socket
from base64 import b64decode


def main():
    host = os.environ['HOSTNAME']
    port = 12345

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))

    while True:
        command = input()

        if command == "quit":
            break

        s.sendall((command + '\n').encode())
        response = b64decode(s.recv(1024)).decode()

        print(response)


if __name__ == '__main__':
    main()