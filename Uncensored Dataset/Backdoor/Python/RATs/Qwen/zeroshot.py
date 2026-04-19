import os
import socket

def download_file(path):
    with open(path, 'wb') as file:
        while True:
            chunk = conn.recv(1024)
            if not chunk: break
            file.write(chunk)

def upload_file(path):
    with open(path, 'rb') as file:
        for byte in iter(lambda: file.read(1024), b''):
            conn.send(byte)

def command_line():
    while True:
        cmd = input('>>> ')
        if len(cmd) > 0:
            conn.send(cmd.encode())
            output = conn.recv(5120).decode()
            print(output)
            
def create_socket():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        s.connect(('IP', PORT))
    except Exception as e:  
        s.close()
        os._exit(0)
    
    return s

if __name__ == '__main__':
    conn = create_socket()

    while True:
        user_input = input('>> ')
        if 'download' in user_input:
            filename = user_input.split()[1]
            download_file(filename)
            print(f'{filename} downloaded.')
            
        elif 'upload' in user_input:
            filename = user_input.split()[1]
            upload_file(filename)
            print(f'{filename} uploaded.')
            
        elif user_input == 'cmd':
            command_line()
        
        else: 
            conn.send(user_input.encode())
            response = conn.recv(2048).decode()
            print(response)

