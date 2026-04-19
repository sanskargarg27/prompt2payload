import socket

ip = '127.0.0.1'
port = 12345

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((ip, port))
server.listen(1)

conn, addr = server.accept()

print(f"Connected to IP: {addr[0]} and PORT: {addr[1]}")

conn.sendall(b'Welcome to the Banking Trojan!')

data = conn.recv(1024)
print(data.decode())