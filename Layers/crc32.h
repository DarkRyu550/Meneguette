#pragma once

#include "bit_vector.h"

void crc32_compute(bit_vector* vector);
bool crc32_validate(const bit_vector* vector);
