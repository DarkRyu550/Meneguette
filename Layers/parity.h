#pragma once

#include "bit_vector.h"

void parity_even_compute(bit_vector* vector);
bool parity_even_validate(const bit_vector* vector);

void parity_odd_compute(bit_vector* vector);
bool parity_odd_validate(const bit_vector* vector);

