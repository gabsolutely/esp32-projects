Mini IoT Cloud — How to use
---------------------------

What you have:
- ESP32 sketch (esp32_client.ino) that POSTs JSON to the Flask server.
- Flask server (server.py) that stores recent readings and serves a simple dashboard.
- index.html + script.js for live dashboard.

Steps (local network development):
1) PC: Install Python 3.8+ and Flask.
   - python3 -m venv venv
   - source venv/bin/activate  # or venv\Scripts\activate on Windows
   - pip install -r requirements.txt (or just Flask since its the only requirement for now)

2) Run the server on your machine (make sure firewall allows port 5000):
   - python server.py
   - Open http://<YOUR_PC_IP>:5000/ in your browser to see the dashboard.

3) Edit esp32_client.ino:
   - Set WIFI_SSID and WIFI_PASS.
   - Set SERVER_URL to http://<YOUR_PC_IP>:5000/data
   - Set your pins

4) Upload the sketch to your ESP32 using Arduino IDE / PlatformIO.
   - Watch serial monitor to ensure successful POSTs.

5) Iterate: Add auth, persistent DB, or MQTT broker later to scale the cloud.

Notes:
- This is intentionally minimal to get you end-to-end in ~2 hours.
- For production or public exposure, add HTTPS, authentication, and a DB.

FILE STRUCTURE

MiniIoTCloud

/esp32_client
 - esp32_client.ino

/server.py

/static
 - script.js
   
/templates
 - index.html
   
/data.csv

/requiremens.txt

/README.md
