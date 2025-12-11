# model/trainModel.py (to train the model, run once)
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestClassifier
import joblib
import os

data_path = os.path.join(os.path.dirname(__file__), 'data.csv')
data = pd.read_csv(data_path)

X = data[['temperature', 'humidity']]
y = data['status']
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

model = RandomForestClassifier(n_estimators=100, random_state=42)
model.fit(X_train, y_train)

joblib.dump(model, r'') # put the path where you want to save the model
print("Model has been trained and saved")