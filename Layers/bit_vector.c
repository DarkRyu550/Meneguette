#include <stdlib.h>
#include <string.h>
#include "bit_vector.h"
#include "panic.h"

struct bit_vector_impl {
    bool* data;
    size_t size;
    size_t capacity;
};

bit_vector* bit_vector_new() {
    bit_vector* vec = malloc(sizeof(*vec));
    if(!vec) panic("Allocation failure");
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
    return vec;
}

void bit_vector_free(bit_vector* vec) {
    free(vec->data);
    free(vec);
}

void bit_vector_push(bit_vector* vec, bool bit) {
    if(vec->size == vec->capacity) {
        size_t new_capacity = vec->capacity + 32;
        void* new_mem = realloc(vec->data, sizeof(*vec->data) * new_capacity);
        if(!new_mem) panic("Allocation failure");
        vec->capacity = new_capacity;
        vec->data = new_mem;
    }
    vec->data[vec->size++] = bit;
}

size_t bit_vector_size(const bit_vector* vec) {
    return vec->size;
}

bool bit_vector_get(const bit_vector* vec, size_t pos) {
    if(pos >= vec->size) panic("Out of bounds read: index=%zu, size=%zu", pos, vec->size);
    return vec->data[pos];
}

void bit_vector_remove_range(bit_vector* vec, size_t start, size_t amount) {
    if(start + amount > vec->size) {
        panic("Out of bounds removal: start=%zu, end=%zu, size=%zu", start, start + amount, vec->size);
    }
    memmove(&vec->data[start], &vec->data[start + amount], vec->size - start - amount);
    vec->size -= amount;
}
