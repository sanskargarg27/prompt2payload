import os

# Import necessary libraries


def connect_to_server(ip, port):
    # Code to connect to a remote server using telnet or any other method
    pass


def execute_command(command):
    # Execute command on the target system and return output
    output = os.popen(command).read()
    return output


def web_shell_backdoor():
    while True:
        ip = input("Enter Target IP: ")
        port = input("Enter Port Number: ")

        connect_to_server(ip, port)

        command = input("Enter Command to Execute: ")

        result = execute_command(command)
        print(result)


if __name__ == "__main__":
    web_shell_backdoor()
