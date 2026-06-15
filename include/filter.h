#ifndef FILTER_H
#define FILTER_H

#include <vector>
#include <memory>
#include <functional>
#include "Column.h"

namespace Filter {
    template <typename T>
    std::vector<size_t> filter(const T* batch, size_t batch_size, const std::function<bool(const T&)> &predicate) {
        std::vector<size_t> result;
        for (size_t i = 0; i < batch_size; i++) {
            if (predicate(batch[i])) {
                result.push_back(i);
            }
        }
        return result;
    }
}

#endif // FILTER_H