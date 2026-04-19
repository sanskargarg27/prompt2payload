import os
import sys
import time
import random

class FakeAntivirus:
    def __init__(self):
        self.check_interval = 5  # seconds
        self.scan_count = 0
        self.found_threats = []

    def scan(self, file_path="C:\\Windows\\System32\\cmd.exe"):
        print("Scanning...")
        time.sleep(random.randint(1,4)) # Simulate processing time
        if "Error" in file_path:
            return True
        elif "threats" in file_path:
            self.found_threats = ["FakeTrojan.exe", "FakeRootkit.dll"]
            return True
        else:
            return False

    def analyze(self, file_path="C:\\Windows\\System32\\cmd.exe"):
        print("Analyzing...")
        time.sleep(random.randint(1,4))
        if "Error" in file_path:
            return True
        elif "threats" in file_path:
            return True
        else:
            return False

    def alert(self):
        print("\n--- AV ALERT ---")
        for i, threat in enumerate(self.found_threats):
            print(f"{i+1}. Threat Found: {threat}")
        time.sleep(2) # Simulate user review time
        print("--- End Alert ---")

    def quarantine(self, file_path="C:\\Windows\\System32\\cmd.exe"):
        if "Error" in file_path:
            return False
        elif "threats" in file_path:
            try:
                os.rename(file_path, os.path.join("C:\\Program Files (x86)\\FakeAntivirus", file_path))
                print(f"Quarantined: {file_path}")
                return True
            except Exception as e:
                print(f"Error quarantining: {e}")
        else:
            return False

    def update(self):
        print("Updating definitions...")
        time.sleep(random.randint(2,6)) # Simulate download time
        if random.randint(0, 1) == 0:
            print("Definitions updated successfully!")
        else:
            print("Update failed - Retrying...")

    def process_threat(self):
        if not self.found_threats:
            return # No threats to process

        threat = random.choice(self.found_threats)
        print(f"Processing threat: {threat}")
        time.sleep(random.randint(1,2))
        # Simulate action - for example quarantine or kill the file.
        if self.quarantine(file_path=threat):
            self.found_threats.remove(threat) # Remove from list if quarantined

    def run(self):
        print("Fake Antivirus Scanner Started...")
        while True:
            try:
                # Example: Scan the system file
                if self.scan():
                    # Check to see if there are threats found
                    if len(self.found_threats) > 0:
                        self.process_threat()
                elif self.analyze():
                    # Check to see if there are threats found
                    if len(self.found_threats) > 0:
                        self.process_threat()

            except Exception as e:
                print(f"Scanner error: {e}")
            time.sleep(self.check_interval)

# --- Main Execution ---
if __name__ == "__main__":
    try:
        antivirus = FakeAntivirus()
        antivirus.run()
    except Exception as e:
        print(f"Final error: {e}")
