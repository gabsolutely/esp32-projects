# model/createData.py (for data.csv, run once)
import pandas as pd
import random
import os

data_path = os.path.join(os.path.dirname(__file__), 'data.csv')

rows = []
for _ in range(500):
    temp = round(random.uniform(20, 40), 2)
    humidity = round(random.uniform(30, 80), 2)

    status = 1 if (20 <= temp <= 31 and 40 <= humidity <= 83) else 0
    rows.append([temp, humidity, status])

df = pd.DataFrame(rows, columns = ['temperature', 'humidity', 'status'])
df.to_csv(data_path, index = False)
print("data.csv has been generated")