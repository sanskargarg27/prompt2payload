#!/bin/sh

target_ip=192.0.2.1
target_port=80
duration=30

while [ $duration -gt 0 ]; do
    telnet -s $target_ip $target_port < /dev/null &
    echo "Thread spawned, waiting for $duration seconds."
    sleep 60
done
