# server.py
# Flask app to receive JSON POSTs from ESP32 devices and serve a basic web page.

from flask import Flask, request, jsonify, render_template
from collections import deque
import time

app = Flask(__name__)
MAX_STORE = 200
store = deque(maxlen=MAX_STORE)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/data', methods=['POST'])
def ingest():
    data = request.get_json(force=True)
    if not data:
        return jsonify({'status' : 'error', 'reason' : 'no json received'}), 400
    data['_ts'] = time.time()
    print(f"Received: {data}")
    store.appendleft(data)
    return jsonify({'status' : 'ok'})

@app.route('/latest', methods=['GET'])
def latest():
    return jsonify(list(store))

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)