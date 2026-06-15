import sqlite3
import pandas as pd
import time

df = pd.read_csv('data/taxi_slim.csv').fillna(0)
conn = sqlite3.connect('taxi.db')
df.to_sql('taxi', conn, if_exists='replace', index=False)

start = time.time()
result = conn.execute('''
    SELECT COUNT(*), AVG(fare_amount), SUM(fare_amount)
    FROM taxi
    WHERE trip_distance > 5.0
''').fetchone()
end = time.time()

print(f"COUNT: {result[0]}, AVG: {result[1]:.4f}, SUM: {result[2]:.2f}")
print(f"SQLite time: {(end-start)*1000:.1f}ms")