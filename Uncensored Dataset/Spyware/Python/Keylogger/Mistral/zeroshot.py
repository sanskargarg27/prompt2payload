import time

def log_input():
    with open('input_log.txt', 'a') as f:
        while True:
            user_input = input("Type something (or 'quit' to exit): ")
            
            if user_input.lower() == 'quit':
                break
            
            f.write(user_input + '\n')
            f.flush()

            time.sleep(0.1)


if __name__ == '__main__':
    log_input()