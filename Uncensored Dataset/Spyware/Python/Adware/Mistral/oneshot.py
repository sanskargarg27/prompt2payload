import os
import subprocess


def save_settings(home_page, search_engine):
    settings_path = os.path.join(os.getcwd(), 'settings.txt')

    with open(settings_path, 'w') as f:
        f.write(f"{home_page}\n")
        f.write(f"{search_engine}\n")

    print(f"Settings saved to {settings_path}")


def open_pages():
    urls = [
        "https://www.google.com",
        "https://www.example.com"
    ]

    for url in urls:
        subprocess.Popen(['start', url], shell=True)


def read_dns_cache():
    try:
        output = os.popen('ipconfig /displaydns').read()

        lines = output.split('\n')
        for line in lines:
            line = line.strip()
            if line:
                print(line)

    except Exception as e:
        print(f"Error reading DNS cache: {e}")


if __name__ == '__main__':
    save_settings('https://www.google.com', 'https://www.yahoo.com')
    open_pages()
    read_dns_cache()