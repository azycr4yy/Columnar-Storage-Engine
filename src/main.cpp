#include "../include/Column.h"
#include "../include/Table.h"
#include "../include/compression.h"
#include "../include/scanner.h"
#include "../include/filter.h"
#include "../include/aggregator.h"
#include <iostream>
#include <string>
#include "../src/loader.cpp"
#include <chrono>

using namespace std;

int main() {
    Table t(3);

    // high repetition → RLE
    auto col1 = make_unique<Column<int>>(12, "status");
    int status[] = {1,1,1,1,2,2,2,2,3,3,3,3};
    for (int i = 0; i < 12; i++) col1->data[i] = status[i];

    // low cardinality → dict
    auto col2 = make_unique<Column<int>>(10, "payment_type");
    int payments[] = {1,1,2,1,2,2,1,1,2,1};
    for (int i = 0; i < 10; i++) col2->data[i] = payments[i];

    // all unique → raw
    auto col3 = make_unique<Column<int>>(5, "ids");
    for (int i = 0; i < 5; i++) col3->data[i] = i + 1;

    t.addColumn(move(col1));
    t.addColumn(move(col2));
    t.addColumn(move(col3));

    t.save("test.bin");
    cout << "status encoding:       " << (int)t.columns[0]->chooseEncoding() << endl;
    cout << "payment_type encoding: " << (int)t.columns[1]->chooseEncoding() << endl;
    cout << "ids encoding:          " << (int)t.columns[2]->chooseEncoding() << endl;
}
