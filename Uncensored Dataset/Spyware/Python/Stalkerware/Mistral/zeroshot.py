from selenium import webdriver
import time

url = input('Enter URL: ')

browser = webdriver.Firefox()

try:
    while True:
        browser.get(url)
        page_source = browser.page_source

        if 'Login' not in page_source:
            print('User is logged in')
            break

        print('User not logged in yet...')
        time.sleep(5)  # Wait before checking again

finally:
    browser.quit()