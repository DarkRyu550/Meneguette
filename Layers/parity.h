#pragma once

#include "bit_vector.h"

/**
 * Computes and appends to the vector the parity bit for even parity.
 *
 * @param vector Data to compute parity of.
 */
void parity_even_compute(bit_vector* vector);

/**
 * Tests whether or not the parity of the data in the vector is valid.
 *
 * @param vector Vector of data + parity to validate.
 *
 * @return True if the parity is valid, false otherwise
 */
bool parity_even_validate(const bit_vector* vector);

/**
 * Computes and appends to the vector the parity bit for odd parity.
 *
 * @param vector Data to compute parity of.
 */
void parity_odd_compute(bit_vector* vector);

/**
 * Tests whether or not the parity of the data in the vector is valid.
 *
 * @param vector Vector of data + parity to validate.
 *
 * @return True if the parity is valid, false otherwise
 */
bool parity_odd_validate(const bit_vector* vector);

