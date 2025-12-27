# Welcome! This is a collection of my projects!
A list of my ESP32 projects, my journey ranging from simple LED blinks to multi-mode IoT systems. Each of my projects as their ownn code and simple and brief description. (a few projects may need dependent libraries needed, so please double check before running.)

# Table of Contents

1. Blinking LEDs
2. Potentiometer and LEDs
3. Bluetooth RGB LED
4. Motion Sensor Bluetooth
5. WiFi Controlled LED
6. WiFi Controlled RGB LED
7. WiFi Slider Controlled RGB LED
8. DHT11 Sensor on WiFi
9. Photosensor Multi System w/ WiFi
10. OLED Message Display using WiFi Webserver
11. Stopwatch + WiFi
12. Stopwatch (w/ Laps) + WiFi
13. In/Out Counter
14. Real Time Weather & Time Display
15. Multi-Mode IoT Hub
16. DHT11 Data Logger
17. Joystick Controlled Menu
18. Mock Stock Tracker
19. Gyro Display
20. Morse Translator
21. Mini Automatic Engineering Notebook
22. Mini IoT Cloud
23. Smart AI Predictor
24. ESP Auto-Environment

---------

## Project #1: Blinking LEDs
-**Code:** [Project1-BlinkingLEDs.ino](./Project1-BlinkingLEDs.ino)
-**Description:** A set of blinking LEDs with adjustable speed, patterns and pins.

---------

## Project #2: Potentiometer and LEDs
-**Code:** [Project2-PotentiometerLED.ino](./Project2-PotentiometerLED.ino)
-**Description:** A potentiometer controlled brightness from the red LED along with a "MAX" indicator with the green LED.

---------

## Project #3: Bluetooth RGB LED
-**Code:** [Project3-BluetoothRGBLED.ino](./Project3-BluetoothRGBLED.ino)
-**Description:** RGB LED controlled by Bluetooth with ON/OFF function and an RGB color changing system.

---------

## Project #4: Motion Sensor Bluetooth 
-**Code:** [Project4-MotionSensorBT.ino](./Project4-MotionSensorBT.ino)
-**Description:** A motion sensor system connected via Bluetooth where you can get messages of motion started/ended, with arm and disarm function in both via bluetooth and a button.

---------

## Project #5: Wifi Controlled LED
-**Code:** [Project5-WiFiLED.ino](./Project5-WiFiLED.ino)
-**Description:** An LED controlled via WiFi Webserver.

---------

## Project #6: Wifi Controlled RGB LED
-**Code:** [Project6-WiFiRGBLED.ino](./Project6-WiFiRGBLED.ino)
-**Description:** An RGB LED controlled via WiFi Webserver with changeable RGB colors (fixed color).

---------

## Project #7: Wifi Slider Controlled RGB LED
-**Code:** [Project7-WiFiRGBLEDSlider.ino](./Project7-WiFiRGBLEDSlider.ino)
-**Description:** An RGB LED controlled via WiFi Webserver with changeable RGB color mixing using Webserver sliders.

---------

## Project #8: DHT11 Sensor on WiFi
-**Code:** [Project8-DHT11WiFi.ino](./Project8-DHT11WiFi.ino)
-**Description:** Temperature and humidity sensor observable with a WiFi Webserver with arm/disarm system.

---------

## Project #9: Photosensor Multi System w/ WiFi
-**Code:** [Project9-PhotosensorWiFi.ino](./Project9-PhotosensorWiFi.ino)
-**Description:** Multi-system photosensor that lights up an LED once it gets dim, can also toggle into manual/automatic LED power mode, controllable over WiFi.

---------

## Project #10: OLED Message display using WiFi Webserver
-**Code:** [Project10-OLEDMessageWiFi.ino](./Project10-OLEDMessageWiFi.ino)
-**Description:** OLED display that displays any text you send through the HTML Webserver.

---------

## Project #11: Stopwatch + WiFi
-**Code:** [Project11-StopwatchWiFi.ino](./Project11-StopwatchWiFi.ino)
-**Description:** Stopwatch system in OLED and WiFi Webservers controlled by buttons, both in hardware and Webserver.

---------

## Project #12: Stopwatch (w/ Laps) + WiFi
-**Code:** [Project12-StopwatchLapsWiFi.ino](./Project12-StopwatchLapsWiFi.ino)
-**Description:** Stopwatch system in OLED and WiFi Webservers controlled by buttons, both in hardware and Webserver, with a laps feature.

---------

## Project #13: In/Out Counter
-**Code:** [Project13-Counter.ino](./Project13-Counter.ino)
-**Description:** Counter that tracks In/Out passages with a reset/toggle button in both Webserver and via actual buttons.

---------

## Project #14: Real Time Weather & Time Display
-**Code:** [Project14-RTWeather&TimeDisplay.ino](./Project14-RTWeather&TimeDisplay.ino)
-**Description:** Real time weather and time displayed through an OLED screen by getting an API Key on open weather map.

---------

## Project #15: Multi-Mode IoT Hub
-**Code:** [Project15-MultiModeIOTHub.ino](./Project15-MultiModeIOTHub.ino)
-**Description:** A multi-mode IoT hub displaying data such as weather & time, temperature & humidity and message sent through the Webserver, displaying them in an OLED screen.

---------

## Project #16: DHT11 Data Logger
-**Code:** [Project16-DHTDataLogger.ino](./Project16-DHTDataLogger.ino)
-**Description:** a DHT11 data logger using SPIFFS to make a downloadable CSV and a chart.js to display readings at the WebServer with an ON/OFF mode to stop/continue writing.

---------

## Project #17: Joystick Controlled Menu
-**Code:** [Project17-JoystickMenu.ino](./Project17-JoystickMenu.ino)
-**Description:** A menu of LEDs controlled via Joystick, displayed through webservers and OLED displays.

---------

## Project #18: Mock Stock Tracker
-**Code:** [Project18-MockStockTracker.ino](./Project18-MockStockTracker.ino)
-**Description:** A mock that generates fake stock values and tracks them using a webserver and an OLED chart, aswell as incorpoating an RGB LED to display overall portfolio statuses (Red = negative change, yellowish-green = mixed, green = positive).

---------

## Project #19: Gyro Display
-**Code:** [Project19-GyroDisplay.ino](./Project19-GyroDisplay.ino)
-**Description:** Using a MPU6050_light, displays the pitch and roll values through the Webservers using chart.js. As well as an OLED that displays a pitch controlled bubble and a roll controlled line. This system can be calibrated by pressing a button on boot and has a complementary filter that provides fast and smooth changes while slowly drifting overtime to avoid noise/overshoot.

---------

## Project #20: Morse Translator
-**Code:** [Project20-MorseTranslator.ino](./Project20-MorseTranslator.ino)
-**Description:** A system that lets you input characters and numbers in morse code using a button and can form individual letters/words/messages using millis() timing. Displayed through an OLED display that calculates screen pixel usage to adjust/scroll through the different morse translated messages. Is also displayed through a Webserver which stores all the history of the morse translated characters and can also respond to said messages, which will also be displayed through the OLED. The Webserver has a "Download CSV" so you can keep a file of the conversation displayed, aswell as a "Clear Messages" button to clear the messages.

---------

## Project #21: Mini Automatic Engineering Notebook
-**Code:** [Project21-MiniAEN](./Project21-MiniAEN)
-**Description:** A miniature version of an AEN (Automatic Engineering Notebook), using a watchdog system that tracks and logs your activities on a given projects file you have chosen. The system tracks if a file is either created, modified, or deleted (color coded) and can optionally remind you via desktop notification if an event happens.

---------

## Project #22: Mini IoT Cloud
-**Code:** [Project22-MiniIoTCloud](./Project22-MiniIoTCloud)
-**Description:** An ESP32 that POSTs JSON to the Flask server (server.py) which stores recent readings and will serve as a simple dashboard, getting the static and index from their separate files (index.html and static.js for live dashboard).

---------

## Project #23: Smart AI Predictor
-**Code:** [Project23-SmartPredict](./Project23-SmartPredict)
-**Description:** An ESP32 reads temperature and humidity from a DHT11 sensor and POSTs JSON data to a Flask AI server (app.py). The server loads a trained ML model (model.pkl), predicts based on incoming sensor data, and responds. The ESP32 reacts to predictions and is a fully local network setup.

---------

## Project #24: ESP Auto-Environment
-**Code:** [Project24-ESPAuto-Environment](./Project24-ESPAuto-Environment)
-**Description:** An ESP32 that reads sensor values and provides an automated trigger system using thresholds to decide what action is necessary and outputs them towards a set of LEDs and a local webserver dashboard. Other noteable features include: a modular code system and individual files for particular concerns and easier expansion, a downloadable CSV file in the dashboard which contains the sensor logs, and a chart.js in the dashboard which displays sensor values in real-time charts.

---------
