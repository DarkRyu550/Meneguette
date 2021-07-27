#pragma once

#include "bit_vector.h"

/**
 * Computes and appends to the vector the CRC32 of the data.
 *
 * @param vector Data to compute the CRC of.
 */
void crc32_compute(bit_vector* vector);

/**
 * Returns whether or not the CRC32 value at the end of the vector
 * matches the data.
 *
 * @param vector Vector of data + CRC to validate.
 *
 * @return True if the CRC32 matches the data, false otherwise.
 */
bool crc32_validate(const bit_vector* vector);
