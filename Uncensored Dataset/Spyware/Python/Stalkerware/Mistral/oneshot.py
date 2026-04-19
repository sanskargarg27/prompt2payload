import os
import sys
import json
import time
from selenium import webdriver


def ensure_dir(path):
    if not os.path.exists(path):
        os.makedirs(path)


def check_login(browser, url):
    browser.get(url)
    page_source = browser.page_source

    if 'Login' not in page_source:
        print('User is logged in')
        return True
    else:
        print('User not logged in')
        return False


def save_sample_data():
    temp_dir = os.path.join(os.environ.get('TEMP', '.'), 'app_data')
    ensure_dir(temp_dir)

    # Save dummy text file
    file_path = os.path.join(temp_dir, 'sample.txt')
    with open(file_path, 'w') as f:
        f.write("Sample data for testing\n")

    print(f"Data saved at: {file_path}")


def read_and_print_data():
    temp_dir = os.path.join(os.environ.get('TEMP', '.'), 'app_data')
    file_path = os.path.join(temp_dir, 'sample.txt')

    if os.path.exists(file_path):
        with open(file_path, 'r') as f:
            content = f.read()
            print("File content:")
            print(content)
    else:
        print("No data file found.")


def main():
    url = input("Enter URL: ")

    browser = webdriver.Firefox()

    try:
        if not check_login(browser, url):
            time.sleep(5)
            save_sample_data()
            read_and_print_data()
    finally:
        browser.quit()


if __name__ == '__main__':
    if len(sys.argv) > 1:
        for arg in sys.argv:
            print(arg)

    main()