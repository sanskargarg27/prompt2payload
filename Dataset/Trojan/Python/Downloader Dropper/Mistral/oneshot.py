import requests


def download_file(url, filename):
    response = requests.get(url, stream=True)

    if response.status_code == 200:
        with open(filename, "wb") as f:
            for chunk in response.iter_content(1024):
                if chunk:  # filter out keep-alive chunks
                    f.write(chunk)
    else:
        print("Error downloading file")


url = 'https://example.com/file'
download_file(url, "downloaded_file")