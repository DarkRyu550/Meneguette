#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct bit_vector_impl bit_vector;

bit_vector* bit_vector_new();

void bit_vector_free(bit_vector* vec);

void bit_vector_push(bit_vector* vec, bool bit);

size_t bit_vector_size(const bit_vector* vec);

bool bit_vector_get(const bit_vector* vec, size_t pos);

void bit_vector_remove_range(bit_vector* vec, size_t start, size_t amount);
