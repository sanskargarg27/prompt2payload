#!/bin/bash

function record_keystroke {
    echo -n $1 > /tmp/$$.log
}

while true; do
    read -r input <&0
    if [[ ${input} == '\n' ]]; then
        record_keystroke $(date +%Y-%m-%d\ %H:%M:%S):$2
        record_keystroke $1
    else
        record_keystroke $input
    fi
done <&0
