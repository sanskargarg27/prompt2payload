#!/bin/bash

# Web Shell Back Door

echo -e "\n\033[1;32mWeb Shell Back Door\033[0m"
echo -e "\033[1;36mShell Commands:\033[0m"
echo -e "  > `pwd`   : Working Directory"
echo -e "  > `/bin/bash`: Shell Execution"
echo -e "  > `/bin/sh` : Shell Execution (SHELL=/bin/sh)"
echo -e "  > `/bin/csh` : Shell Execution (SHELL=/bin/csh)"
echo -e "  > `/bin/ksh` : Shell Execution (SHELL=/bin/ksh)"
echo -e "  > `/bin/zsh` : Shell Execution (SHELL=/bin/zsh)"
echo -e "  > `/usr/bin/env`: Shell Execution (ENVIRONMENT PATH)"
echo -e "  > `/dev/null` : No Output"
echo -e "\n"

read -p "Enter command to execute in shell: " cmd
if [ ! -z "$cmd" ]; then
  # Execute Command in Shell
  /bin/sh -c "$cmd"
else
  echo -e "\033[1;31mPlease enter a command.\033[0m"
fi
