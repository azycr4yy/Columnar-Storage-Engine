import pandas as pd
df = pd.read_parquet('yellow_tripdata_2023-01.parquet')
df.to_csv('taxi.csv', index=False)