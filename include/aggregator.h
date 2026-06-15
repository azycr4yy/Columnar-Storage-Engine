#ifndef aggregator_H
#define aggregator_H

#include <vector>
#include <memory>
#include <functional>
#include "Column.h"

template<typename T>
struct AggResult {
    T sum;
    size_t count;
    double avg;
};

namespace Aggregator {
    template<typename T>
    AggResult<T> aggregate(const T* batch , size_t size,const std::vector<size_t>* indices = nullptr ){
        AggResult<T> result{0, 0, 0.0};
        if (indices) {
            for (size_t idx : *indices) {
                result.sum += batch[idx];
                result.count++;
            }
        } else {
            for (size_t i = 0; i < size; ++i) {
                result.sum += batch[i];
                result.count++;
            }
        }
        if (result.count > 0) {
            result.avg = static_cast<double>(result.sum) / result.count;
        }
        return result;
    }

    }

#endif // aggregator_H