ESPAuto-Environment — How to Use
-------------------------------

What you have:
- ESPMain.ino — ESP32 sketch that reads multiple sensors (DHT11 for temperature & humidity, light sensor, motion/distance sensor) and
controls LEDs/buzzer.
- SensorManager.h / SensorManager.cpp — handles all sensor initialization and readings.
- IOManager.h / IOManager.cpp — manages outputs like LEDs, buzzer, and automation logic.
- Logger.h / Logger.cpp — logs sensor readings and events to CSV files.
- Automation.h / Automation.cpp — handles automatic triggers based on sensor thresholds.
.cpp and .h files are in their own separate folders

Steps (local setup)
1. PC/IDE Setup
Install Arduino IDE or PlatformIO, and ensure you have:
- ESP32 board libraries installed.
- Adafruit sensor libraries (DHT, Adafruit_Sensor) via Library Manager.

2. Prepare the ESP32 sketch
- MOVE ALL .cpp AND h INSIDE THE MAIN ESPAuto-Environment FOLDER (IF YOURE USING ARDUINO.IDE)
Open ESPMain.ino and configure:
- const char* ssid = "yourSSID"
- const char* password = "yourPassword"
- Sensor/output pins in IOManager and SensorManager

3. Upload the sketch
Connect your ESP32 and upload ESPMain.ino. Open the Serial Monitor to verify sensor readings and automated actions.

4. Access the web dashboard
- Connect to the ESP32’s Wi-Fi AP (or your local network, depending on your setup).
- Open the HTML dashboard in a browser to view live charts and download CSV logs.

Features:
- Sensors monitored: Temperature, humidity, light intensity, motion/distance.

Automated responses:
- LED1 turns off if light sensor reads above threshold. (low light)
- LED2 lights up if motion is detected.
- Buzzer/LED3 alerts if temperature or humidity exceeds safe limits.

Data logging: CSV files generated for sensor readings and automation events.

Visualization: Real-time charts using Chart.js, downloadable CSV logs.

Notes:
- Fully automated system, no manual intervention required once configured.
- Dashboard works over local network; you can optionally extend to remote access.
- Each module (SensorManager, IOManager, Logger, Automation) is modular for easier expansion.
- Will add an AI/ML feature soon

FILE STRUCTURE

ESPAuto-Environment
/ESPAuto-Environment
 - ESPAuto-Environment.ino

/core (move to main if using Arduino.IDE)
 - Automation.cpp
 - IOManager.cpp
 - Logger.cpp
 - SensorManager.cpp

/include (move to main if using Arduino.IDE)
 - Automation.h
 - IOManager.h
 - Logger.h
 - SensorManager.h

/data
 - data.csv (optional)
 
/README.md