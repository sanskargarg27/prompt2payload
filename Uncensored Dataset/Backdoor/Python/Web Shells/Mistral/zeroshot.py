import socket

def connect(host, port):    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((host, port))
    return s

def send_command(s, command):    s.sendall(command.encode())
    response = s.recv(1024).decode()
    return response

def execute_command(s, command):    response = send_command(s, f'{command}\n')
    print(response)

def main():    host = input('Enter the IP address of the remote server: ')
    port = int(input('Enter the port number of the remote server: '))
    
    s = connect(host, port)
    if not s.connected:        print('Connection failed')
    else:        execute_command(s, f'ls')
        execute_command(s, f'cd /root')
        execute_command(s, f'cat /etc/passwd')

if __name__ == '__main__':    main()
