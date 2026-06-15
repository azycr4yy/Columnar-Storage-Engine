#ifndef TABLE_H
#define TABLE_H

#include "headers.h"
#include "Column.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>

class Table {
private:
    int m_max_rows; 
public:
    std::vector<std::unique_ptr<ColumnBase>> columns;
    int columnCount;
    int columnReserved;
    Table(int columnCount){
        this->columnCount = columnCount;
        this->columnReserved = 0;
        this->m_max_rows = 0;
        columns.reserve(columnCount);
    }
    void addColumn(std::unique_ptr<ColumnBase> col){
        if (columnReserved >= columnCount) {
            throw std::runtime_error("Cannot add more columns than reserved");
        }
        columns.push_back(std::move(col));
        columnReserved++;
    }
    void print() const{
        for (const auto& col : columns) {
            col->print();
        }
    }
    void save(const std::string& filename) const{
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Could not open file: " + filename);
        }
        FileHeader header;
        header.magic = magic;
        header.num_columns = columnCount;
        header.num_rows = m_max_rows;
        file.write(reinterpret_cast<const char*>(&header), sizeof(header));
        for (const auto& col : columns)
        {
            ColumnHeader colHeader;
            std::string name = col->getName();
            strncpy(colHeader.name, name.c_str(), sizeof(colHeader.name) - 1);
            colHeader.name[sizeof(colHeader.name) - 1] = '\0';
            colHeader.dtype = col->getType();
            colHeader.encoding = col->chooseEncoding();
            colHeader.uncompressed_size = col->getSize();
            colHeader.compressed_size = col->getSize();
            colHeader.offset = 0;
            file.write(reinterpret_cast<const char*>(&colHeader), sizeof(colHeader));
            col->writeData(file);
        }
        file.close();
    }
    void load(const std::string& filename){
        std::ifstream file(filename,std::ios::binary);
        if (!file){
        throw std::runtime_error("Could not open file: " + filename);
        }
        FileHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (header.magic != magic){
            throw std::runtime_error("Invalid magic number");
        }
        for (int i = 0; i < header.num_columns; i++) {
            ColumnHeader colHeader;
            file.read(reinterpret_cast<char*>(&colHeader), sizeof(colHeader));
            std::string name(colHeader.name);
            std::unique_ptr<ColumnBase> col;
            int size = colHeader.uncompressed_size;
            if (colHeader.dtype == 0) {
                col = std::make_unique<Column<int>>(size, name);
            } else if (colHeader.dtype == 1) {
                col = std::make_unique<Column<float>>(size, name);
            } else if (colHeader.dtype == 2) {
                col = std::make_unique<Column<double>>(size, name);
            } else if (colHeader.dtype == 3) {
                col = std::make_unique<Column<std::string>>(size, name);
            } else {
                throw std::runtime_error("Invalid column type");
            }
            columns.push_back(std::move(col));
            columns.back()->readData(file);
        }
        
        file.close();

    }
};

#endif