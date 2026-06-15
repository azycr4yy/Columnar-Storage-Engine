import pandas as pd
data = pd.read_csv(r"C:\Users\Udayan\Desktop\udayan\vscode\Columnar storage engine\data\taxi_slim.csv")
print(data.columns.to_list())
print(data.shape)