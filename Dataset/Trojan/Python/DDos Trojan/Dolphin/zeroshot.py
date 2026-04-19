import socket
import time

target_url = "http://example.com"  # Replace this with your target URL


def send_packets():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(("0.0.0.0", 80))  # Change the IP and port as needed

    try:
        while True:
            data = "GET / HTTP/1.1\r\nHost: {}\r\n\r\n".format(target_url).encode("utf-8")
            client.sendall(data)

            time.sleep(0.5)  # Adjust the sleep time as needed
    except KeyboardInterrupt:
        pass

    finally:
        client.close()


if __name__ == "__main__":
    send_packets()
