import os
import time

def replace_file(path, new_content):
    with open(path, 'w') as f:
        f.write(new_content)

# Replace the content of a file with a random string
replace_file('/path/to/your/file', 'Random String')