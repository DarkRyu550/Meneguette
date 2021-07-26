#pragma once

#include "bit_vector.h"
#include <stdint.h>

void bitconv_push_u8(bit_vector* vec, uint8_t val);

void bitconv_push_u32be(bit_vector* vec, uint32_t val);

void bitconv_push_string(bit_vector* vec, const char* str);

uint8_t bitconv_read_u8(const bit_vector* vec, size_t bit_pos);

uint32_t bitconv_read_u32be(const bit_vector* vec, size_t bit_pos);

char* bitconv_read_string(const bit_vector* vec, size_t bit_pos);
