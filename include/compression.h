#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <map>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "column.h"




namespace Compression {
    template<typename T>
    std::pair<std::map<T, size_t>, std::vector<size_t>> dict_encode(const Column<T>& col) {
        std::map<T, size_t> dict;
        std::vector<size_t> encoded;
        size_t index = 0;
        for (size_t i = 0; i < col.getSize(); i++) {
            T value = col.data[i];
            if (dict.find(value) == dict.end()) {
                dict[value] = index++;
            }
            encoded.push_back(dict[value]);
        }
        return {dict, encoded};
    }

    template<typename T>
    std::vector<std::pair<T, size_t>> rle_encode(const Column<T>& col) {
        std::vector<std::pair<T, size_t>> encoded;
        if (col.getSize() == 0) {
            throw std::invalid_argument("Column size must be greater than 0");
        }
        for (size_t i = 0; i < col.getSize();) {
            T value = col.data[i];
            size_t count = 1;
            while (i + count < col.getSize() && col.data[i + count] == value) {
                count++;
            }
            encoded.emplace_back(value, count);
            i += count;
        }
        return encoded;
    }
    template<typename T>
    Column<T> rle_decode(const std::vector<std::pair<T, size_t>>& encoded) {
        if (encoded.empty()) {
            throw std::invalid_argument("Encoded data cannot be empty");
        }
        size_t total = 0;
        for (const auto& [value, count] : encoded) total += count;
        Column<T> result(total);
        size_t idx = 0;
        for (const auto& [value, count] : encoded) {
            for (size_t i = 0; i < count; i++) {
                result.data[idx++] = value;
            }
        }
        return result;
    }
    template<typename T>
    uint8_t chooseEncoding(const Column<T>& col) 
    {
        std::unordered_set<T> unique(col.data.get(), col.data.get() + col.size);
        double cardinality = (double)unique.size() / col.size;

        size_t runs = 1;
        for (size_t i = 1; i < col.size; i++) {
            if (col.data[i] != col.data[i-1]) runs++;
        }
        double avg_run = (double)col.size / runs;

        if (avg_run >= 4.0) return 1;       // RLE
        if (cardinality < 0.5) return 2;    // dictionary
        return 0;                            // raw
}
}

#endif
