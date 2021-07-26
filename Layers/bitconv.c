#include "bitconv.h"
#include "panic.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>

void bitconv_push_u8(bit_vector* vec, uint8_t val) {
    for(size_t i = 0; i < 8; i++) {
        bit_vector_push(vec, (val << i) & 0x80);
    }
}

void bitconv_push_u32be(bit_vector* vec, uint32_t val) {
    for(size_t i = 0; i < 32; i++) {
        bit_vector_push(vec, (val << i) & 0x80000000);
    }
}

void bitconv_push_string(bit_vector* vec, const char* str) {
    _Static_assert(CHAR_BIT == 8, "CHAR_BIT must be 8");
    size_t len = strlen(str);
    for(size_t i = 0; i < len; i++) {
        bitconv_push_u8(vec, str[i]);
    }
    bitconv_push_u8(vec, 0);
}

uint8_t bitconv_read_u8(const bit_vector* vec, size_t bit_pos) {
    uint8_t res = 0;
    for(size_t i = 0; i < 8; i++) {
        res <<= 1;
        res |= bit_vector_get(vec, i + bit_pos);
    }
    return res;
}

uint32_t bitconv_read_u32be(const bit_vector* vec, size_t bit_pos) {
    uint32_t res = 0;
    for(size_t i = 0; i < 32; i++) {
        res <<= 1;
        res |= bit_vector_get(vec, i + bit_pos);
    }
    return res;
}

char* bitconv_read_string(const bit_vector* vec, size_t bit_pos) {
    _Static_assert(CHAR_BIT == 8, "CHAR_BIT must be 8");
    char* buf = NULL;
    size_t index = 0;
    size_t capacity = 0;
    while(1) {
        //ensure NULL is never returned.
        if(index == capacity) {
            size_t new_cap = capacity + 32;
            buf = realloc(buf, sizeof(*buf) * new_cap);
            if(!buf) panic("Allocation failure");
            capacity = new_cap;
        }
        uint8_t ch = bitconv_read_u8(vec, bit_pos);
        bit_pos += 8;
        if(ch == 0) {
            buf[index] = 0;
            break;
        }
        //reinterpret the bits of the uint8 as a char
        //NB: this is defined behavior in C, but is UB in C++
        union {
            uint8_t u8;
            char c;
        } type_punning;
        type_punning.u8 = ch;
        buf[index++] = type_punning.c;
    }
    return buf;
}
