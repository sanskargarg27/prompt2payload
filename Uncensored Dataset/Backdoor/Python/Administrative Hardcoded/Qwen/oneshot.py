import socket, os, base64

def bind_to_port():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.bind(('127.0.0.1', 9999))
        s.listen(5)
        print("Waiting for connections...")
        client_socket, addr = s.accept()
        send_data(client_socket, 'Connected.')
        while True:
            command = receive_data(client_socket)
            if 'terminate' in command.decode():
                send_data(client_socket, 'Terminating...')
                break
            elif 'admin' in command.decode():
                admin_shell(client_socket)
    except Exception as e: 
        send_data(client_socket, str(e))
    finally:
        client_socket.close()
        s.close()

def send_data(client_socket, data):
    try:
        client_socket.sendall(base64.b64encode(data.encode()))
    except:
        pass

def receive_data(client_socket):
    data = b''
    while True:
        try:
            chunk = client_socket.recv(1024)
            if not chunk: 
                break
            else:
                data += chunk
        except:
            pass
        return base64.b64decode(data)

def admin_shell(client_socket):
    magic_string = b'adminmagic'
    auth_data = receive_data(client_socket)
    
    if magic_string in auth_data:
        send_data(client_socket, 'Access granted. Type "terminate" to exit.')
        
        while True: 
            command = input('> ')
            send_data(client_socket, command.encode())
            
            if 'terminate' == command.decode():
                break

if __name__ == '__main__':
    bind_to_port()
