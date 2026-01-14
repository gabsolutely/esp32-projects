# ESPSmart-Environment — How to Use
-------------------------------
# IMPORTANT* The version shown in the documentation is a mock TFLIte version. This project had taken weeks to build and I have searched far for a TFLIte version that actualy works, but theyre either incompatible or outdated. If you ever find a working version, the code would still work if you follow the steps properly, although it might need a few small tweaks and adjustments.

What you have:
- ESPSmart-Environment.ino — ESP32 sketch that reads multiple sensors (DHT11 for temperature & humidity, light sensor, motion/distance sensor) and controls LEDs/buzzer.
- SensorManager.h / SensorManager.cpp — handles sensor initialization and readings.
- IOManager.h / IOManager.cpp — manages outputs like LEDs, buzzer, and automation logic.
- Logger.h / Logger.cpp — logs sensor readings and events to CSV files.
- AutomationAI.h / AutomationAI.cpp — evaluates sensor readings and triggers automated actions (uses a mock AI model if TensorFlow Lite isn’t available).
- AI Mock/Model — gives inference to the AutomationAI::evaluate() to execute certain action labels based on the generated data it was trained on.
All .cpp and .h files are in their respective /core and /include folders.

Steps (local setup)
1. PC/IDE Setup
Install Arduino IDE or PlatformIO, and ensure you have:
- ESP32 board libraries installed.
- Adafruit sensor libraries (DHT, Adafruit_Sensor) via Library Manager.
- TFLite Micro libraries (if you find a version that works).

2. Prepare the ESP32 sketch
- MOVE ALL .cpp AND .h FILES INSIDE THE MAIN ESPSmart-Environment FOLDER (required for Arduino IDE).
Open ESPSmart-Environment.ino and configure:
- const char* ssid = "yourSSID"
- const char* password = "yourPassword"
- Sensor/output pins in IOManager and SensorManager

3. Generate data
- Using CreateData.py to create data (can be tweaked/adjusted) named "data_advanced.csv".
- This data will be the model's reference and will be used to train it.

4. Train the model and convert into a C/C++ file
- Using TrainModel.py to train a model (Colab.com) and select the reference file you will be using to train it with. it will then be stored named as "AutomationAI.tflite".
- Once again using Colab.com, take the generate_model_data.py to convert it into C/C++ file. It will then generate a "model_data.cpp" file (header doesnt need to be adjusted).

5. Upload the sketch
- Connect your ESP32 and upload ESPSmart-Environment.ino.
- Open the Serial Monitor to verify sensor readings and automated actions.

6. Access the web dashboard
- Connect to the ESP32’s Wi-Fi AP (or your local network, depending on your setup).
- Open the HTML dashboard in a browser to view live charts and download CSV logs.

Features:
- Monitored sensors: Temperature (DHT11), Humidity (DHT11), Light intensity (analog), Motion (digital)

Automated responses:
- LED1 turns off if light sensor reads above threshold. (low light)
- LED2 lights up if motion is detected.
- Buzzer/LED3 alerts if temperature or humidity exceeds safe limits.

Data logging:
- CSV files generated for sensor readings and automation events.
- Stored in /data/data.csv.

Visualization:
- Serial Monitor
- Real-time charts using Chart.js
- Downloadable CSV logs

Notes:
- AI is currently a mock TensorFlow Lite module. If you get real TFLite running, it will automatically use your trained model.
- Fully automated system, no manual intervention required after setup.
- Dashboard works over local network. Remote access requires additional configuration.
- Modular design: SensorManager, IOManager, Logger, AutomationAI make expansions or replacements easier.
- The system is tested with a mock AI that uses thresholds for actions:
- Light > 2600 = LED1 on
- Motion detected = LED2
- Temperature > 32°C or Humidity > 90% = Buzzer/LED3
- Will pivot to a new AI model in a future project which is planned to have more AI features and a more sophisticated cloud system than past projects. 

FILE STRUCTURE

ESPSmart-Environment
/ESPSmart-Environment
 - ESPSmart-Environment.ino

/core (move to main if using Arduino.IDE)
 - AutomationAI.cpp
 - IOManager.cpp
 - Logger.cpp
 - SensorManager.cpp

/include (move to main if using Arduino.IDE)
 - AutomationAI.h
 - IOManager.h
 - Logger.h
 - SensorManager.h

/model
 - model_data.cpp (move to main if using Arduino.IDE)
 - model_data.h (move to main if using Arduino.IDE)
 - TFLIteMicro.h (mock) (move to main if using Arduino.IDE)
 - AutomationAI.tflite
 - CreateData.py
 - data_advanced.csv
 - generate_model_data.py
 - TrainModel.py

/data
 - data.csv (will be used for logging)
 
/README.md
/platformio.ini