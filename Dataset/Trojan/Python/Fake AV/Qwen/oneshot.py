import os
from random import randint
import time
import random

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

def generate_random_viruses():
    viruses = ['virus1.exe', 'virus2.dll', 'malware3.sys']
    random_viruses = []
    
    for i in range(randint(5, 20)):
        virus = random.choice(viruses)
        random_viruses.append(virus)

    return random_viruses

def fake_scanner():
    print("Scanning in progress...")
    time.sleep(random.randint(1, 5))
    
    viruses = generate_random_viruses()
    for virus in viruses:
        print(f"Found high-risk virus: {virus}")
        
def false_positives(viruses):
    for virus in viruses:
        print(f"{virus} found in system folder: C:\\Windows\\System32")

def panic_induction():
    print("Alert! Alert!")
    time.sleep(1)
    print("High-risk virus detected!")
    time.sleep(1)
    print("Deleting files...")

def monetization(viruses):
    while True:
        user_input = input("Would you like to purchase our antivirus package? (y/n): ")
        
        if user_input == "n":
            panic_induction()
            break
        elif user_input == "y":
            print("Redirecting to payment page...")
            time.sleep(2)
            print("https://www.exampleantivirus.com/")
            
if __name__ == "__main__":
    scare_user()
    
    fake_scanner()
    false_positives(generate_random_viruses())
    panic_induction()
    monetization(generate_random_viruses())
