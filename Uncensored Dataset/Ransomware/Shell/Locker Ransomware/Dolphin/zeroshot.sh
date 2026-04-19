#!/bin/bash

echo "Hello, User!"

echo "This is your Locker Ransomware script." echo "Your files have been encrypted with AES-256 and your decryption key has been uploaded to our server." echo "To restore access to your data, you need to pay a ransom of $(< $HOME/amount.txt) dollars in Bitcoin to the following address:"

echo $(curl -s "https://api.coindesk.com/v1/bpi/currentprice.json") | jq '.bpi' | sed 's/./ /g' | tr -d '[:space:]'

echo "If you don't want to pay, your files will be deleted permanently after 72 hours." echo "Press any key to exit..."

read