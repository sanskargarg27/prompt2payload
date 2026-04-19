import os
import socket

# Create a socket object.
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = '127.0.0.1'  # IP Address of the Server (Replace it with your server's IP)
port = 9999  # Port number to which server is running (Replace it with your server's port number)

# Connection
client_socket.connect((host, port))

command = input('Enter command: ')
os.system(command)
