#ifndef SCANNER_H
#define SCANNER_H

#include <cstddef>
#include <vector>
#include <fstream>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include "Column.h"


template <typename T>
class Scanner {
public:
    size_t batch_size;
    size_t cursor;
    size_t size;
    const Column<T>& column;

    Scanner(const Column<T>& column, size_t batch_size = 4096, size_t start_pos = 0)
        : batch_size(batch_size), cursor(start_pos), size(column.getSize()), column(column)
    {
    }

    bool hasNextBatch() const {
        return cursor < size;
    }

    std::pair<const T*, size_t> nextBatch() {
        if (!hasNextBatch()) {
            throw std::runtime_error("No more batches available");
        }
        size_t remaining = column.getSize() - cursor;
        size_t current_batch_size = std::min(batch_size, remaining);
        const T* data_ptr = column.data.get() + cursor;
        cursor += current_batch_size;
        return std::make_pair(data_ptr, current_batch_size);
    }
};

#endif // SCANNER_H