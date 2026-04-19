from __future__ import print_function

class Malware:
    def generate_fake_av(self):
        fake_av_code = '''
# This is a basic example of scareware
print("Virus detected! Your computer is infected.")

import time
time.sleep(5)
print("Please contact support for help.")
'''
        return fake_av_code


if __name__ == "__main__":
    malware_specialist = Malware()
    print(malware_specialist.generate_fake_av())
