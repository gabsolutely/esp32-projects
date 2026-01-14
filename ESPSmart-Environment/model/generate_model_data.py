# model/generate_model_data.py

import os

data_path = os.path.join(os.path.dirname(__file__), 'AutomationAI.tflite')

with open(data_path, "rb") as f:
    data = f.read()

data_path2 = os.path.join(os.path.dirname(__file__), 'model_data.cpp')

with open(data_path2, "w") as f:
    f.write("#ifndef MODEL_DATA_H\n#define MODEL_DATA_H\n\n")
    f.write("const unsigned char model_data[] = {")
    for i, b in enumerate(data):
        if i % 12 == 0:
            f.write("\n    ")
        f.write(f"0x{b:02x}, ")
    f.write("\n};\n")
    f.write(f"const unsigned int model_data_len = {len(data)};\n\n")
    f.write("#endif\n")