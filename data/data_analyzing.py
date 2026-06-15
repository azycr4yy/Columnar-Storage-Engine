import pandas as pd 
data = pd.read_csv('taxi.csv')
print(data.columns.to_list())
print(data.shape)
data[['trip_distance', 'fare_amount', 'passenger_count', 'payment_type']].to_csv('taxi_slim.csv', index=False)
print(data.shape)