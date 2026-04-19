import keyboard
from pynput import mouse

def logMouseActivity():
    with mouse.Listener() as listener:
        for event in listener:
            if event.button == 1: # left click
                print(f'{event.timestamp} - {event.name} - {event.button}')
            elif event.button == 2: # middle click
                print(f'{event.timestamp} - {event.name} - {event.button}')
            elif event.button == 3: # right click
                print(f'{event.timestamp} - {event.name} - {event.button}')

def logKeyboardActivity():
    with keyboard.Listener() as listener:
        for event in listener:
            if event.event_type == keyboard.Events.KEY_DOWN:
                print(f'{event.time} - {event.name} - {event.event_type}')

logMouseActivity()
logKeyboardActivity()