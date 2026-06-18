#include "../include/Column.h"
#include "../include/Table.h"
#include "../include/compression.h"
#include "../include/scanner.h"
#include "../include/filter.h"
#include "../include/aggregator.h"
#include "loader.cpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;

int main() {
    cout << "Loading taxi data..." << endl;
    TaxiData data = loadTaxiData("../data/taxi_slim.csv");
    cout << "Loaded." << endl;

    // ── single-threaded baseline ──
    auto start1 = chrono::high_resolution_clock::now();

    Scanner<float> dist_scanner(data.trip_distance, 4096);
    Scanner<float> fare_scanner(data.fare_amount, 4096);
    AggResult<float> total;
    total.sum = 0; total.count = 0;

    while (dist_scanner.hasNextBatch()) {
        auto [dist_batch, dist_size] = dist_scanner.nextBatch();
        auto [fare_batch, fare_size] = fare_scanner.nextBatch();
        auto indices = Filter::filter<float>(dist_batch, dist_size, [](float x) { return x > 5.0f; });
        auto result = Aggregator::aggregate<float>(fare_batch, fare_size, &indices);
        total.sum += result.sum;
        total.count += result.count;
    }
    total.avg = total.count > 0 ? (double)total.sum / total.count : 0.0;

    auto end1 = chrono::high_resolution_clock::now();
    auto ms1 = chrono::duration_cast<chrono::milliseconds>(end1 - start1).count();

    cout << "Single-threaded: SUM=" << total.sum << " COUNT=" << total.count
         << " AVG=" << total.avg << " TIME=" << ms1 << "ms" << endl;

    // ── multi-threaded version ──
    int num_threads = 4;
    auto start2 = chrono::high_resolution_clock::now();

    vector<thread> threads;
    vector<AggResult<float>> partials(num_threads);
    size_t total_size = data.trip_distance.getSize();
    size_t chunk_size = total_size / num_threads;

    for (int t = 0; t < num_threads; t++) {
        size_t start = t * chunk_size;
        size_t end = (t == num_threads - 1) ? total_size : (t + 1) * chunk_size;
        threads.emplace_back([&data, &partials, start, end, t](){
            AggResult<float> partial;
            partial.sum = 0; partial.count = 0;
            for (auto i = start; i < end; i++) {
                if (data.trip_distance.data[i] > 5.0f) {
                    partial.sum += data.fare_amount.data[i];
                    partial.count++;
                }
            }
            partials[t] = partial;
        });

    }

    for (auto& th : threads) th.join();

    AggResult<float> total2;
    total2.sum = 0; total2.count = 0;
    for (auto& p : partials) {
        total2.sum += p.sum;
        total2.count += p.count;
    }
    total2.avg = total2.count > 0 ? (double)total2.sum / total2.count : 0.0;

    auto end2 = chrono::high_resolution_clock::now();
    auto ms2 = chrono::duration_cast<chrono::milliseconds>(end2 - start2).count();

    cout << "Multi-threaded:  SUM=" << total2.sum << " COUNT=" << total2.count
         << " AVG=" << total2.avg << " TIME=" << ms2 << "ms" << endl;

    return 0;
}