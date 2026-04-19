import socket

# Define remote connection details
remote_ip = '127.0.0.1'   # Replace with actual IP
remote_port = 12345       # Replace with actual port


def connect():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((remote_ip, remote_port))
    return s


def send_command(s, command):
    s.sendall(command.encode())
    response = s.recv(1024).decode()
    print(response)
    return response


def main():
    s = connect()

    while True:
        command = input('> ')
        if command.lower() in ('exit', 'quit'):
            break

        send_command(s, command)

    s.close()


if __name__ == '__main__':
    main()