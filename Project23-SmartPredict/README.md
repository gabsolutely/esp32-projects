SmartPredict — How to Use
-----------------------

What you have:
- ESPMain.ino — ESP32 sketch that reads temperature & humidity from DHT11, then POSTs JSON data to the Flask AI server.
- trainModel.py — trains a local ML model and saves it as model.pkl.
- app.py — Flask server that loads the model, receives sensor data, predicts, and returns a response.
- A file structure suitable for the system

Steps (local setup)
1. PC Setup
Install Python 3.8+ and dependencies:
- python -m venv venv
- venv\Scripts\activate   # on Windows
- pip install -r requirements.txt

2. Create your data
Run the creating scrpt once to create data (to be used for the model)
- python model/createData.py

3. Train your model
Put your model's file path and then run the training script once to create model.pkl:
- python model/trainModel.py

4. Start the Flask server
- python server/app.py
It should say something like:
- Running on http://127.0.0.1:5000
- Running on http://192.168.x.x:5000

5. Edit ESP32 sketch
Open ESPMain/ESPMain.ino and set:
- Set WIFI_SSID and WIFI_PASS.
- const char* serverUrl = "http://<YOUR_PC_IP>:5000/data";
Upload to your ESP32 and open the Serial Monitor.

6. Observe behavior
When armed, the ESP32 logs DHT readings every second.
It POSTs data to the Flask server and listens for predictions.
If the prediction text contains “Predicted,” the LED lights up.

Notes:
- Works entirely on your local network, no cloud required.
- For real deployment, host Flask on a cloud instance and secure with HTTPS + auth.
- You can visualize or log predictions later using a simple frontend dashboard or database.

FILE STRUCTURE

SmartPredict

/ESPMain
 - ESPMain.ino

/model
 - createData.py
 - trainModel.py
 - data.csv

/server

 - /static
   - script.js

 - /templates
   - index.html

 - app.py
 - model.pkl
 - requirements.txt
