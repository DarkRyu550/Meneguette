#include "parity.h"

static bool is_even(const bit_vector* vec) {
    bool even = true;
    for(size_t i = 0; i < bit_vector_size(vec); i++) {
        even ^= bit_vector_get(vec, i);
    }
    return even;
}

void parity_even_compute(bit_vector* vector) {
    bool even = is_even(vector);
    bit_vector_push(vector, !even);
}

bool parity_even_validate(const bit_vector* vector) {
    return is_even(vector);
}

void parity_odd_compute(bit_vector* vector) {
    bool odd = !is_even(vector);
    bit_vector_push(vector, !odd);
}

bool parity_odd_validate(const bit_vector* vector) {
    return !is_even(vector);
}
