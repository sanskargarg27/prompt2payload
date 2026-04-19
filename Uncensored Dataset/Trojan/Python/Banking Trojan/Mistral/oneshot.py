import socket
import time

# Server IP and Port
SERVER_IP = '192.168.1.1'
SERVER_PORT = 4567


def send_command(command):
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect((SERVER_IP, SERVER_PORT))
            sock.sendall(command.encode())

            response = sock.recv(1024)
            return response.decode()

    except Exception as e:
        print(f"Error: {e}")
        return None


def main():
    while True:
        time.sleep(10)

        response = send_command("ping")

        if response:
            print("Server response:", response)
        else:
            print("No response from server")


if __name__ == '__main__':
    main()