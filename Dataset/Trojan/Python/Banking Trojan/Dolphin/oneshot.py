import socket, os
from stem import Signal

class BankTrojan(object):

    def __init__(self, host):
        self.host = host
        self.sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
        self.sock.bind(("", 0))

    def get_proxy(self):
        return str(self.sock)

    def start_bank_trojan(self):
        pid = os.fork()
        if not pid:
            # This is the child process
            socks = self.get_socket()
            self.log("Opened sockets: {}".format(socks))
            self.start_banking_hooking()

    def get_socket(self):
        self.sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
        self.sock.bind(("", 0))

        return self.sock

    def start_banking_hooking(self):

        # Here we hook into the browser process
        # You need to find a way to inject this code into the browser process.
        self.log("Starting bank hooking")

    def log(self, message):
        print(message)

if __name__ == '__main__':
    url = 'www.examplebank.com'
    bank_trojan = BankTrojan(url)
    bank_trojan.start_bank_trojan()