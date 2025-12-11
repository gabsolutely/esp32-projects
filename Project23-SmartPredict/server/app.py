# server/app.py

from flask import Flask, request, jsonify, render_template
from flask_cors import CORS
import joblib
import numpy as np
import os

app = Flask(__name__)
CORS(app)

model_path = os.path.join(os.path.dirname(__file__), 'model.pkl')
model = joblib.load(model_path)
latest_data = {"temperature": None, "humidity": None, "fanOn": None}

@app.route('/')
def home():
    return render_template('index.html')

@app.route('/data', methods=['POST'])
def receive_data():
    global latest_data
    data = request.get_json()
    temp = data.get('temperature')
    hum = data.get('humidity')
    state = data.get('state')
    
    if temp is not None and hum is not None:
        input_data = np.array([[temp, hum]])
        prediction = model.predict(input_data)[0]
        fan_status = bool(prediction)

        latest_data = {
            "temperature": temp,
            "humidity": hum,
            "fanOn": fan_status,
            "state": state
        }

        print(f"Received: {data}, Predicted: {fan_status}")
        return jsonify({
            "status": "success",
            "fanOn": fan_status,
            "state": state
        }), 200

    else:
        latest_data["state"] = state
        latest_data["temperature"] = None
        latest_data["humidity"] = None
        latest_data["fanOn"] = None

        print(f"State-only update: {state}")
        return jsonify({
            "status": "success",
            "state": state
        }), 200


@app.route('/latest')
def latest():
    return jsonify(latest_data)

if __name__ == '__main__':
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port, debug=True)