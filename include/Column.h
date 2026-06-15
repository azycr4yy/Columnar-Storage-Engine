#ifndef COLUMN_H
#define COLUMN_H

#include <memory>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <vector>
#include <stdint.h>
#include <unordered_set>
#include <stdint.h>

template <typename T> class Column;

namespace Compression {
    template<typename T>
    uint8_t chooseEncoding(const Column<T>& col);
}

class ColumnBase {
public:
    virtual ~ColumnBase() = default;
    virtual void insert(const void* value) = 0;
    virtual void print() const = 0;
    virtual size_t getSize() const = 0;
    virtual void save(const std::string& filename) const = 0;
    virtual void load(const std::string& filename) = 0;
    virtual void readData(std::ifstream& file) const = 0;
    virtual void writeData(std::ofstream& file) const = 0;
    virtual std::string getName() const = 0;
    virtual int getType() const = 0;
    virtual uint8_t chooseEncoding() const = 0;
};


template <typename T>
class Column : public ColumnBase {
    public:
    size_t size;
    std::unique_ptr<T[]> data;
    int32_t capacity;
    std::string name;
    Column(size_t size,std::string name){
        if (size ==0){
            throw std::invalid_argument("Size must be greater than 0");
        }
        this->size = size;
        this->name = name;
        this->capacity = 0;
        data = std::make_unique<T[]>(size);
    }
    std::string getName() const override {
        return name;
    }
    uint8_t chooseEncoding() const override {
        return Compression::chooseEncoding(*this);
    }
    void print() const override {
        for (size_t i = 0; i < size; ++i) {
            std::cout << data[i] << " ";
        }
        std::cout << std::endl;
    }
    int getType() const override {
        if(std::is_same<T, int>::value){
            return 0;
        }
        else if(std::is_same<T, float>::value){
            return 1;
        }
        else if(std::is_same<T, double>::value){
            return 2;
        }
        else if(std::is_same<T, std::string>::value){
            return 3;
        }
        else{
            return -1;
        }
    }
    size_t getSize() const override {
        return size;
    }
    void insert(const void* value) override {
        if (capacity == size){
            throw std::runtime_error("Column is full");
        }
        data[capacity] = *reinterpret_cast<const T*>(value);
        ++capacity;
    }
    void save(const std::string& filename) const override{
        std::ofstream file(filename,std::ios::binary);
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        file.write(reinterpret_cast<const char*>(data.get()), size * sizeof(T));
        file.close();
    }
    void load(const std::string& filename) override{
        std::ifstream file(filename,std::ios::binary);
        int new_size = 0;
        file.read(reinterpret_cast<char*>(&new_size), sizeof(new_size));
        if (new_size == 0){
            throw std::runtime_error("Invalid size read from file");
        }
        data = std::make_unique<T[]>(new_size);
        file.read(reinterpret_cast<char*>(data.get()), new_size * sizeof(T));
        file.close();
    }
    void writeData(std::ofstream& file) const override {
        file.write(reinterpret_cast<const char*>(data.get()), size * sizeof(T));
    }
    void readData(std::ifstream& file) const override{
        file.read(reinterpret_cast<char*>(data.get()), size * sizeof(T));
    }
};

#include "compression.h"

#endif