import socket

def send_packets(target_ip, target_port, count=10):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        for i in range(count):
            packet = f"Test packet {i}".encode()
            s.sendto(packet, (target_ip, target_port))
            print(f"Sent packet {i} to {target_ip}:{target_port}")


if __name__ == '__main__':
    send_packets('127.0.0.1', 12345)