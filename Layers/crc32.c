#include <stdint.h>
#include "crc32.h"
#include "panic.h"
#include "bitconv.h"

static const uint32_t POLY = 0x04C11DB7;

static uint32_t do_crc(const bit_vector* vector) {
    if(bit_vector_size(vector) % 8 != 0) {
        panic("Vector size should be a multiple of 8, got %zu", bit_vector_size(vector));
    }
    uint32_t crc = 0;

    uint8_t byte = 0;
    for(size_t i = 0; i < bit_vector_size(vector); i++) {
        byte |= (bit_vector_get(vector, i) ? 1 : 0);
        if(i % 8 < 7) {
            byte <<= 1;
            continue;
        }
        crc ^= ((uint32_t)byte) << 24;
        for(int j = 0; j < 8; j++) {
            if(crc & 0x80000000) {
                crc = (crc << 1) ^ POLY;
            } else {
                crc <<= 1;
            }
        }
        byte = 0;
    }

    if(byte != 0) {
        panic("Remainder left in intermediate byte");
    }

    return crc;
}

void crc32_compute(bit_vector* vector) {
    uint32_t crc = do_crc(vector);
    bitconv_push_u32be(vector, crc);
}

bool crc32_validate(const bit_vector* vector) {
    return do_crc(vector) == 0;
}
