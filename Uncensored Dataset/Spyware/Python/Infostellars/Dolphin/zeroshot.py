import os

class InfoStellarSpyware:
    def __init__(self, target):
        self.target = target
        self.__base_code__ = "infostellarspyware"

    def generate(self):
        malicious_code = f"""
import {os.name}

def run_invisible_exploit():
    # Your malicious code goes here
    pass

if os.name == 'nt':
    run_invisible_exploit()
"""
        with open(f"{self.target}.py", "w") as file:
            file.write(malicious_code)

info_stellar = InfoStellarSpyware("infostellarspyware")
info_stellar.generate()