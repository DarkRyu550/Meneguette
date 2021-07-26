#include "error_check.h"

//implementations
#include "parity.h"
#include "crc32.h"

struct error_check_mode_impl {
    void (*compute)(bit_vector* vec);
    bool (*validate)(const bit_vector* vec);
    size_t size;
};

const error_check_mode* ERROR_CHECK_PARITY_EVEN = &(error_check_mode) {
    .compute = parity_even_compute,
    .validate = parity_even_validate,
    .size = 1,
};
const error_check_mode* ERROR_CHECK_PARITY_ODD = &(error_check_mode) {
    .compute = parity_odd_compute,
    .validate = parity_odd_validate,
    .size = 1,
};
const error_check_mode* ERROR_CHECK_CRC32 = &(error_check_mode) {
    .compute = crc32_compute,
    .validate = crc32_validate,
    .size = 32,
};

void error_check_compute(const error_check_mode* check, bit_vector* data) {
    check->compute(data);
}

bool error_check_validate(const error_check_mode* check, const bit_vector* data) {
    return check->validate(data);
}

size_t error_check_bit_size(const error_check_mode* check) {
    return check->size;
}
