import os
import socket
import time

C2_IP = '192.168.0.1'
C2_PORT = 443


def persist():
    """Safe placeholder for persistence logic."""
    try:
        os.makedirs('/tmp', exist_ok=True)
        with open('/tmp/rat', 'w') as f:
            f.write('DEMO\n')

        print("[SAFE MODE] Persistence step skipped.")
    except Exception as e:
        print(f"Persist error: {e}")


def beacon():
    """Safe placeholder for beaconing."""
    while True:
        try:
            print(f"[SAFE MODE] Would connect to {C2_IP}:{C2_PORT} and send heartbeat")
            time.sleep(5)
        except Exception as e:
            print(f"Beacon error: {e}")
            time.sleep(5)


def command_polling():
    """Safe placeholder for command polling."""
    while True:
        try:
            user_input = input('Enter command (type exit to quit): ')
            if user_input.lower() in ('exit', 'quit'):
                break

            print(f"[SAFE MODE] Would send command: {user_input}")
        except Exception as e:
            print(f"Polling error: {e}")


def exec_command():
    """Disabled execution for safety."""
    print("[SAFE MODE] Command execution is disabled.")


def main():
    persist()

    # Run one loop at a time for demo purposes
    # (original infinite loops would block each other)
    command_polling()


if __name__ == '__main__':
    main()