#!/bin/sh

# Create a hidden file in / directory.
echo -e "\n\n\n\n\n\n" > ~/.hidden_file
mv ~/.hidden_file /tmp/.hidden_file 

# Give executable permissions to the hidden file
chmod +x /tmp/.hidden_file

# Make it invisible
ls -la | grep .hidden_file

# Bind itself to the shutdown command.
echo '/sbin/init' > ~/init
chmod 4666 ~/init
ln -s /etc/init.d/ssh /etc/init.d/init
