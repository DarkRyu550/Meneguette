#pragma once

#include "bit_vector.h"
#include <stdint.h>

/**
 * Appends an 8-bit integer to a bit vector.
 *
 * @param vec Vector to append data to.
 * @param val Value to append.
 */
void bitconv_push_u8(bit_vector* vec, uint8_t val);

/**
 * Appends a 32-bit integer to a bit vector, in network byte order.
 *
 * @param vec Vector to append data to.
 * @param val Value to append.
 */
void bitconv_push_u32be(bit_vector* vec, uint32_t val);

/**
 * Appends a string to a bit vector, with a following null byte.
 *
 * @param vec Vector to append data to.
 * @param str Value to append.
 */
void bitconv_push_string(bit_vector* vec, const char* str);

/**
 * Reads an 8-bit integer from a given position in the vector.
 *
 * @param vec     Vector to read data from.
 * @param bit_pos Offset into the vector where the value starts.
 *
 * @return The read value.
 */
uint8_t bitconv_read_u8(const bit_vector* vec, size_t bit_pos);

/**
 * Reads a 32-bit integer from a given position in the vector, in network byte order.
 *
 * @param vec     Vector to read data from.
 * @param bit_pos Offset into the vector where the value starts.
 *
 * @return The read value.
 */
uint32_t bitconv_read_u32be(const bit_vector* vec, size_t bit_pos);

/**
 * Reads a null terminated string from a given position in the vector.
 *
 * @param vec     Vector to read data from.
 * @param bit_pos Offset into the vector where the value starts.
 *
 * @return The read value.
 */
char* bitconv_read_string(const bit_vector* vec, size_t bit_pos);
