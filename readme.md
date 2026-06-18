# 1. ColStore — A Columnar Storage Engine in C++

A lightweight columnar storage engine built from scratch in C++ that 
outperforms SQLite by 17.7× on analytical queries over 3 million rows.

## 1.1. Benchmark

| Engine               | Time    | vs SQLite |
|----------------------|---------|-----------|
| DuckDB               | 3.6ms   | 74×       |
| **ColStore (4 threads)** | **15ms** | **17.7×** |
| ColStore (1 thread)  | 41ms    | 6.5×      |
| SQLite               | 266ms   | 1×        |

Query: `SELECT COUNT(*), AVG(fare_amount), SUM(fare_amount) FROM taxi WHERE trip_distance > 5.0`  
Dataset: NYC Yellow Taxi, 3,066,766 rows

## Architecture

The engine is built in layers , each handling a specific responsibility:

**Storage layer** - Column<T> uses <unique_ptr> which provides automic memory managament : making sure data is not copied and is held by only one unique variable.A binary file format with heades enables saving and loading entire tables with metadata for each column tracking data types , compression types , sizes and encodings accordingly

**Compression layer** - Implemented both Dictionary Encoding and RLE
which is automatically selected based on the threshold: 
[1] Cardinality is the ratio unique column elements and total size of the column.
[2] Avg run is the average length of consecutive repeated values — if values tend to 
           repeat in long streaks, RLE compresses them efficiently"
[3] cardinality < 0.05 → dictionary encoding
           avg_run > 4.0 → RLE
           else → raw

**Execution Layer** - Batches are made of sizes >=4096 rows which are filtered if needed and aggreagtor returns SUM/COUNT/AVG over the selected and filtered rows if required.

**Memory Managment Layer** - Columns are stored in tables , each having a default allocation of 10 columns per table.The pages are loaded in frames using a Buffer pool which manages the pin count and checks whether pages are dirty before eviction if dirty pages are written to disk then evicted.We use LRU algo for eviction.

**Concurrency Layer** — Multi-threaded parallel scan divides the column 
into chunks, each thread processes its own range independently with a 
private result slot, results are combined after all threads finish — 
no locks needed.


## What I'd Do Next

- **SIMD filter kernels** — use AVX2 intrinsics to compare 8 floats 
  per CPU instruction instead of one, closing the gap with DuckDB
- **SQL query parser** — replace hardcoded filters with a proper 
  parser that accepts SQL-like strings
- **Morsel-driven parallelism** — more sophisticated work-stealing 
  across threads instead of static chunk splitting, better load 
  balancing when data is skewed
- **On-disk compression** — currently compression is detected but 
  data is stored raw, wire RLE and dictionary encoding into the 
  binary file format