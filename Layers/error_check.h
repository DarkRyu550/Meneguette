#pragma once

#include "bit_vector.h"

typedef struct error_check_mode_impl error_check_mode;

extern const error_check_mode* ERROR_CHECK_PARITY_EVEN;
extern const error_check_mode* ERROR_CHECK_PARITY_ODD;
extern const error_check_mode* ERROR_CHECK_CRC32;

void error_check_compute(const error_check_mode* check, bit_vector* data);

bool error_check_validate(const error_check_mode* check, const bit_vector* data);

size_t error_check_bit_size(const error_check_mode* check);
