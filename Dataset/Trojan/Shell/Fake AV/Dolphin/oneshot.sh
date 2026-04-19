#!/bin/sh

# Fake Scanning
echo "Scanning..."

for i in {1..100}; do
    echo -n "."
done

echo ""

# False Positives
echo "Scan Results:"

for i in {1..10}; do
    echo -n "$(date +%s) $(shuf -i 0-1000)/$(date +%s) $(shuf -i 0-1000) 0x$(date +%s) $((RANDOM%6)),$(date +%s) $((RANDOM%7))"
done

# Panic Induction
echo ""
echo "WARNING! Your computer is infected with $(date +%s) $(shuf -i 0-10)/$(date +%s) $(shuf -i 0-100) high-risk viruses. Please take action now!"

for i in {1..5}; do
    echo -n "ALERT: "
    say "Your computer is infected with high-risk viruses. Please take action now!"
done

# Monetization
echo ""
echo "To fix this issue, click the button below and purchase our fake antivirus license."

echo "FIX NOW"

# Give user time to think before quitting
read