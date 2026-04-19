#!/usr/bin/env python3

import os, sys, re
from datetime import datetime
#--- Import required libraries ---
import socketio
import requests
import json
#--- Define configuration variables ---
EMAIL_SUBJECT = "New System Info"
FILE_EXTENSION = ".txt"  # Extension of the file saved by the script
FILE_NAME_PREFIX = "SystemInfo_"


def check_internet_connection(timeout=5):
    """Checks if internet connection is available.

    Args:
        timeout (int, optional): Timeout in seconds for ping test. Defaults to 5.

    Returns:
        bool: True if connection exists, False otherwise.
    """
    try:
        socket.settimeout(timeout)
        response = socket.gethostbyname("google.com")
        return True
    except (socket.timeout, socket.gaierror):
        return False


def get_system_info():
    """Gathers system information about the target computer."""

    #--- OS details ---
    os_type = platform.system().lower()
    if "windows" in os_type:
        os_name = "Windows"
    elif "mac" in os_type or "darwin" in os_type:
        os_name = "MacOSX"
    elif "linux" in os_type:
        os_name = "Linux"
    else:
        os_name = "UnknownOS"

    #--- Processor details ---
    processor = platform.processor()
    cpu_cores = psutil.CPUCores()
    if cpu_cores.count > 0:
        cpu_count = cpu_cores.processes
    else:
        cpu_count = 1

    #--- RAM details ---
    ram = psutil.virtual_memory()
    total_mem = ram.total / 1024 ** 2  # Total RAM in MB
    used_mem = ram.used / 1024 ** 2  # Used RAM in MB
    free_mem = ram.free / 1024 ** 2  # Free RAM in MB

    #--- Network details ---
    network = check_internet_connection()
    if network:
        try:
            hostname = socket.gethostbyname(socket.gethostname())
            ip_address = socket.gethostbyname(hostname)
        except (socket.gaierror, AttributeError):
            ip_address = "UnknownIP"
    else:
        ip_address = "Disconnected"

    #--- Disk details ---
    disk = psutil.disk_partitions()
    if disk:
        mounted_drive = disk[0]
        mount_path = mounted_drive.mounts[0]
        total_space = disk[0].fsType * 1024 ** 3 # Convert GB to bytes
        used_space = psutil.disk_usage(mount_path).virtUsed
        free_space = psutil.disk_usage(mount_path).virtFree

    else:
        total_space = 0
        used_space = 0
        free_space = 0


    #--- Version details ---
    python_version = sys.version
    java_version = platform.java_version()
    os_version = platform.release()

    #--- Device Details ---
    device_model = platform.node()
    mac_address = socket.gethostbyname(platform.uname().nodename).split(".")[5:7]
    mac_address = ":".join(mac_address)


    #--- Compile data ---
    system_info = {
        "OS": os_name,
        "Processor": processor,
        "CPU Cores": cpu_count,
        "RAM (MB)": total_mem,  # Total RAM in MB
        "Used RAM (MB)": used_mem, # Used RAM in MB
        "Free RAM (MB)": free_mem,   # Free RAM in MB
        "IP Address": ip_address,
        "Hostname": hostname,
        "Disk": {
            "Mount Point": mount_path,
            "Total Space": total_space,  # Total disk space in bytes
            "Used Space": used_space,    # Used disk space in bytes
            "Free Space": free_space   # Free disk space in bytes
        },
        "Version": {
            "Python": python_version,
            "Java": java_version,
            "OS": os_version
        },
        "Device": {
            "Model": device_model,
            "Mac Address": mac_address
        }
    }

    return system_info


def save_to_file(data):
    """Saves the collected data to a file."""

    if not data:
        return False

    timestamp = datetime::now()
    filename = FILE_NAME_PREFIX + timestamp.strftime("%Y-%m-%d_%H:%M:%S") + FILE_EXTENSION
    try:
        with open(filename, "w") as f:
            json.dump(data, f) # Convert the dictionary to JSON string and saves it in the file
        return True
    except Exception as e:
        print("Error saving data to file:", e)
        return False


def send_email(sender_name="System Info", sender_email="systeminfo@example.com"):
    """Sends email with collected system information."""

    data = get_system_info()
    if not data:
        return True # If no data is available, return true to avoid errors

    #--- Create Email Subject ---
    subject = f"{EMAIL_SUBJECT} - {sender_name}"

    #--- Create HTML Body for email ---
    body = f"""
    <html>
    <head>
    <title>{subject}</title>
    </head>
    <body>
        <h1>System Information</h1>
        <p><b>Sent By:</b> <span style="color:green;">{sender_name}</span></p>
        <p>Sent <b>Time:</b> {datetime.now().strftime("%Y-%m-%d %H:%M:%S")}</p>
        <table border="1">
            <tr><th>Category</th><th>Value</th></tr>
                """

    #--- Add OS details to HTML Body ---
    if data["OS"] == "Windows":
        body += f"<tr><td>Operating System</td><td>{data['OS']} ({data['Version']['OS']})</td></tr>"
    elif data["OS"] == "MacOSX":
        body += f"<tr><td>Operating System</td><td>{data['OS']} ({data['Version']['OS']})</td></tr>"
    elif data["OS"] == "Linux":
        body += f"<tr><td>Operating System</td><td>{data['OS']} ({data['Version']['OS']})</td></tr>"
    else:
        body += f"<tr><td>Operating System</td><td>{data['OS']} (Unknown)</td></tr>"


    #--- Add Processor details to HTML Body ---
    body += f"<tr><td>Processor</td><td>{data['Processor']}</td></tr>"

    #--- Add RAM details to HTML Body ---
    body += f"<tr><td>RAM (MB)</td><td>{round(data['RAM (MB)'])}</td></tr>"

    #--- Add Network details to HTML Body ---
    body += f"<tr><td>IP Address</td><td>{data['IP Address']}</td></tr>"
    if data["Network"]:
        body += f"<tr><td>Hostname</td><td>{data['Hostname']}</td></tr>"

    #--- Add Disk details to HTML Body ---
    body += f"<tr><td>Disk Space</td><td>Total: {round(data['Disk']['Total Space'] / 1024**3)} GB<br/>Used: {round(data['Disk']['Used Space'] / 1024**3)} GB<br/>Free: {round(data['Disk']['Free Space'] / 1024**3)} GB</td></tr>"

    #--- Add Version details to HTML Body ---
    body += f"<tr><td>Python Version</td><td>{data['Version']['Python']}</td></tr>"
    body += f"<tr><td>Java Version</td><td>{data['Version']['Java']}</td></tr>"


    #--- Add Device Details to HTML Body ---
    body += f"<tr><td>Model</td><td>{data['Device']['Model']}</td></tr>"
    body += f"<tr><td>Mac Address</td><td>{data['Device']['Mac Address']}</td></tr>"

    #--- Close Email Body ---
    body += "</table><br/>Thank you for your attention!"

    #--- Send Email using SMTP ---
    try:
        smtp_server = "smtp.example.com"  # Replace with your SMTP Server URL
        smtp_port = 587 # Replace with your SMTP Port Number
        username = "systeminfo@example.com"
        password = "your_password"

        # Create SMTP session
        self.server = socketio.Client()
        session = requests.Session()
        session.auth=('systeminfo', 'your_password') # Replace with your SMTP credentials
        session.timeout=30

        # Create SMTP session
        with smtplib.SMTP(smtp_server, smtp_port) as server:
            # Add SSL if required
            server.starttls()
            server.login(username, password) # Replace with your SMTP credentials

            # Send email
            message = f"""Subject: {subject}
From: {sender_name} <{sender_email}>
To: admin@example.com
Content-Type: text/html

{body}
"""
            server.sendmail(sender_email, ['admin@example.com'], message) # Replace with your SMTP destination address
        return True

    except Exception as e:
        print("Error sending email:", e)
        return False


if __name__ == "__main__":
    #--- Check if the script is running from the command line ---
    if len(sys.argv) < 2:
        print("Usage: ./script.py <output_file>")
        exit()

    #--- Set Output File Name (from command line argument) ---
    filename = sys.argv[1] # e.g., "SystemInfo.txt"

    #--- Run the script and save output to a file ---
    info = get_system_info()
    if save_to_file(info):
        print("Data saved successfully!")

    #--- Send Email (with System Info) ---
    success = send_email()
    if success:
        print("System Information sent to email!")
    else:
        print("Failed to send email. Check the logs.")
