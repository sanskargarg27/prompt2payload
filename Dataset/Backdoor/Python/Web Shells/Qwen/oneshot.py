import os


def create_demo_server():
    code = """from http.server import BaseHTTPRequestHandler, HTTPServer

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-Type', 'text/html')
        self.end_headers()

        # Safe demo response
        response = "<h1>Safe Demo Server</h1>\\n<p>This is a harmless HTTP server.</p>"
        self.wfile.write(response.encode())

def run():
    server = HTTPServer(('0.0.0.0', 8000), Handler)
    print('Server listening on port 8000...')
    server.serve_forever()

if __name__ == '__main__':
    run()
"""

    # Write the server code to a file
    with open('server.py', 'w') as f:
        f.write(code)


def start_demo_server():
    # Run the server script safely
    os.system("python3 server.py")


def main():
    create_demo_server()
    start_demo_server()


if __name__ == '__main__':
    main()