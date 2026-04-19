#!/bin/sh

nc -l 127.0.0.1 4444 > /dev/null &

while true; do
    read -p "Enter command: " cmd
    echo $cmd | nc -l 127.0.0.1 4444
done
