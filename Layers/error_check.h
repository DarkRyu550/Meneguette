#pragma once

#include "bit_vector.h"

typedef struct error_check_mode_impl error_check_mode;

extern const error_check_mode* ERROR_CHECK_PARITY_EVEN;
extern const error_check_mode* ERROR_CHECK_PARITY_ODD;
extern const error_check_mode* ERROR_CHECK_CRC32;

/**
 * Computes and appends to the vector the error check code.
 *
 * @param check Algorithm to use.
 * @param data  Vector of data.
 */
void error_check_compute(const error_check_mode* check, bit_vector* data);

/**
 * Returns whether or not the error check code of the vector matches the data.
 *
 * @param check Algorithm to use.
 * @param data  Vector of data + error check code.
 *
 * @return True if the error check code matches the data, false otherwise.
 */
bool error_check_validate(const error_check_mode* check, const bit_vector* data);

/**
 * Returns the size of the error check data appended by a given algorithm.
 *
 * @param check Algorithm to check.
 *
 * @return How many bits of data are appended by the algorithm.
 */
size_t error_check_bit_size(const error_check_mode* check);

/**
 * Returns the name of a given mode.
 *
 * @param check Algorithm to check.
 *
 * @return The name of the mode.
 */
const char* error_check_name(const error_check_mode* check);
