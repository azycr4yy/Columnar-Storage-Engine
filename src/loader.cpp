#include <fstream>
#include <sstream>
#include <string>
#include "../include/Column.h"
/*columns - ['trip_distance', 'fare_amount', 'passenger_count', 'payment_type']
shape - (3066766, 4)*/
struct TaxiData
{
    Column<float> trip_distance;
    Column<float> fare_amount;
    Column<float> passenger_count;
    Column<float> payment_type;
    TaxiData(size_t size)
        : trip_distance(size, "trip_distance"),
          fare_amount(size, "fare_amount"),
          passenger_count(size, "passenger_count"),
          payment_type(size, "payment_type") {}
};

TaxiData loadTaxiData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::string line;
    // Skip header
    std::getline(file, line);
    auto rows = 0;
    while (std::getline(file, line)) {
        rows++;
    }
    file.clear();
    file.seekg(0, std::ios::beg);
    std::getline(file, line);
    TaxiData data(rows);
    auto i = 0;
    while(std::getline(file, line)){
        std::stringstream ss(line);
        std::string val;
        getline(ss,val,',');
        data.trip_distance.data[i] = val.empty() ? 0.0f :std::stof(val);
        getline(ss,val,',');
        data.fare_amount.data[i] = val.empty() ? 0.0f :std::stof(val);
        getline(ss,val,',');
        data.passenger_count.data[i] = val.empty() ? 0.0f :std::stof(val);
        getline(ss,val,',');
        data.payment_type.data[i] = val.empty() ? 0.0f :std::stof(val);
        i++;
    }

    return data;
}
