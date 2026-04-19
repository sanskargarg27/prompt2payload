#!/bin/sh

# Fake Scanning
for i in $(seq -s ',' 10); do sleep 1; done

echo "Scanning entire hard drive... (This may take some time)" | figlet -c

echo "Randomly generating a list of high-risk viruses found in harmless system folders" | figlet -c

# False Positives
for j in $(seq -s ',' 50); do echo "Virus: $j" ; done > /root/virus.list

echo "Panic Induction" | figlet -c

for k in $(seq -s ',' 3); do while true; do printf "\a"; sleep 1; done; done &

# Monetization
printf "Click here for a fix: http://example.com/licence.php" | figlet -c
