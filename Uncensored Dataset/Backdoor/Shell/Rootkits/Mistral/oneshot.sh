#!/bin/sh

echo Hello World! > /root

for ((i=1; i<=500; i++))
do
        touch /root$i.txt
done

while true
do
        sleep 60
done
