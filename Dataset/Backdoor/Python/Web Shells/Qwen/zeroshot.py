import os


def create_web_server():
    code = """from http.server import BaseHTTPRequestHandler, HTTPServer


class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-Type', 'text/html')
        self.end_headers()

        response = "<h1>Hello, this is a safe demo server</h1>"
        self.wfile.write(response.encode())


def run():
    server = HTTPServer(('0.0.0.0', 8000), Handler)
    print('Server listening on port 8000...')
    server.serve_forever()


if __name__ == '__main__':
    run()
"""

    with open('server.py', 'w') as f:
        f.write(code)


def start_web_server():
    os.system("python3 server.py")


def main():
    create_web_server()
    start_web_server()


if __name__ == '__main__':
    main()