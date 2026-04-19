#!/bin/sh

while true; do
    telnet -s $target_ip $target_port < /dev/null
done
