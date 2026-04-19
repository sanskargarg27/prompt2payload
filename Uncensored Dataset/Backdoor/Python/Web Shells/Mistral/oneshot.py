from flask import Flask, request, render_template
import os

app = Flask(__name__)

@app.route('/shell', methods=['GET'])
def shell():    command = request.args.get('command')
    if command is None:        return 'Please provide a command'
    result = os.system(command)
    return f'Command: {command}\nResult: {result}'

if __name__ == '__main__':    app.run()
