# model/CreateData.py

import csv
import random

NUM_SAMPLES = 500
CSV_FILE = "data_advanced.csv"

def simulate_sensor():
    # Simulated sensor readings
    temp = round(random.uniform(20.0, 35.0), 2)      # deg C
    humidity = round(random.uniform(30.0, 80.0), 2)  # percent
    light = random.randint(0, 4095)                  # ADC
    motion = random.randint(0, 1)                    # motion: 0 or 1

    # Labeling logic
    # 0 = do nothing, 1 = LED1 on, 2 = LED2 on, 3 = buzzer
    if light > 2600:
        label = 1
    elif temp > 32 or humidity > 90:
        label = 3
    elif motion == 1:
        label = 2 
    else:
        label = 0

    return [temp, humidity, light, motion, label]

with open(CSV_FILE, mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["temp", "humidity", "light", "motion", "label"])
    for _ in range(NUM_SAMPLES):
        writer.writerow(simulate_sensor())

print(f"{NUM_SAMPLES} advanced samples generated in {CSV_FILE}")