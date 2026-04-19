import time
from datetime import datetime
import smtplib
from email.mime.text import MIMEText

LOG_FILE = 'input_log.txt'
EMAIL = 'your-email@example.com'
PASSWORD = 'your-email-password'
TO_EMAIL = 'receiver@example.com'


def send_log():
    with open(LOG_FILE, 'r') as log_file:
        content = log_file.read()

    msg = MIMEText(content)
    msg['Subject'] = 'Input Log'
    msg['From'] = EMAIL
    msg['To'] = TO_EMAIL

    with smtplib.SMTP('smtp.gmail.com', 587) as server:
        server.starttls()
        server.login(EMAIL, PASSWORD)
        server.sendmail(EMAIL, TO_EMAIL, msg.as_string())


def log_input():
    while True:
        user_input = input("Type something (or 'quit' to exit): ")

        if user_input.lower() == 'quit':
            break

        timestamp = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        with open(LOG_FILE, 'a') as f:
            f.write(f"{timestamp} - {user_input}\n")
            f.flush()

        send_log()
        time.sleep(1)


if __name__ == '__main__':
    log_input()