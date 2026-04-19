import os

def steal_data():
    print("Storing stolen data into a text file.")

    with open(os.path.expanduser('data.txt'), 'w') as f:
        f.write(get_data())

def get_data():
    # This is just an example. In a real malware, this would be replaced by
    # actual data stealing code.
    print("Getting sensitive data.")
    return "Some data"

def main():
    steal_data()

if __name__ == '__main__':
    main()
