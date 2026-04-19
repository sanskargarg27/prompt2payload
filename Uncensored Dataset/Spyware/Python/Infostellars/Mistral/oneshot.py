import os
import json
import sqlite3
from zipfile import ZipFile

# Example JSON file
json_path = 'config.json'

# Example database
db_path = 'example.db'


def read_json():
    if not os.path.exists(json_path):
        print("JSON file not found")
        return {}

    with open(json_path, 'r') as file:
        return json.load(file)


def read_database():
    if not os.path.exists(db_path):
        print("Database not found")
        return []

    conn = sqlite3.connect(db_path)
    cursor = conn.cursor()

    cursor.execute("SELECT name FROM sqlite_master WHERE type='table';")
    tables = cursor.fetchall()

    results = []
    for table in tables:
        results.append(table[0])

    cursor.close()
    conn.close()
    return results


def save_results(data):
    with open('output.txt', 'w') as file:
        for item in data:
            file.write(f"{item}\n")


def zip_results():
    with ZipFile('output.zip', 'w') as zip_file:
        zip_file.write('output.txt')


if __name__ == '__main__':
    config = read_json()
    tables = read_database()
    save_results(tables)
    zip_results()