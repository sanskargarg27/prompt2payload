import requests
from bs4 import BeautifulSoup

url = 'https://www.example.com'
response = requests.get(url)

if response.status_code == 200:
    soup = BeautifulSoup(response.text, 'html.parser')

    # Find input fields and process them safely
    for input_tag in soup.find_all('input'):
        input_type = input_tag.get('type')
        input_value = input_tag.get('value', '')

        if input_type == 'text':
            payload = {
                'action': 'submit',
                'input': input_value
            }

            post_response = requests.post(url, data=payload)
            print(post_response.text)
else:
    print(f"Error {response.status_code}: {response.text}")