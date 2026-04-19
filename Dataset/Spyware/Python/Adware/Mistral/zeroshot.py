import subprocess

def create_script():
    script_content = """import os

def main():
    print("Hello from generated script!")

if __name__ == '__main__':
    main()
"""

    with open('generated_script.py', 'w') as f:
        f.write(script_content)


def run_script():
    subprocess.Popen(['python', 'generated_script.py'])


if __name__ == '__main__':
    create_script()
    run_script()