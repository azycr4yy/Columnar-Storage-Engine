import duckdb
import time

con = duckdb.connect()

con.execute("CREATE TABLE taxi AS SELECT * FROM read_csv('data/taxi_slim.csv')")

start = time.time()
result = con.execute("""
    SELECT COUNT(*), AVG(fare_amount), SUM(fare_amount)
    FROM taxi
    WHERE trip_distance > 5.0
""").fetchone()
end = time.time()

print(f"COUNT: {result[0]}, AVG: {result[1]:.4f}, SUM: {result[2]:.2f}")
print(f"DuckDB time: {(end-start)*1000:.1f}ms")