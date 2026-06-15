#include <stdint.h>

uint32_t magic = 0x53544F52;

struct FileHeader {
    char name[32];
    uint32_t magic = 0x53544F52;
    uint16_t version = 1;
    uint16_t num_columns;
    uint64_t num_rows;
};
struct ColumnHeader {
    char name[32];        // column name, fixed size for simplicity
    uint8_t dtype;        // 0 = int, 1 = float, 2 = double, 3 = string
    uint8_t encoding;     // 0 = raw, 1 = RLE, 2 = dictionary
    uint64_t offset;      // where this column's data starts in the file
    uint64_t compressed_size;
    uint64_t uncompressed_size;
};