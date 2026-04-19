import socket
import time

def send_command(ip, port, command):
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((ip, port))
    client.sendall(command.encode())  # send as bytes

    response = receive_response(client)
    client.close()
    return response


def receive_response(client):
    response = b''
    while True:
        data = client.recv(1024)
        if not data:
            break
        response += data
    return response.decode()


while True:
    command = input("Enter command: ")

    if command == 'quit':
        break
    else:
        output = send_command('192.168.1.1', 4567, command)
        print("Response:", output)