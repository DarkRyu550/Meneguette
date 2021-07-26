#include "bitconv.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    bit_vector* vec = bit_vector_new();

    bitconv_push_string(vec, "ABCD EFGH");

    bit_vector_remove_range(vec, 8, 24);
    char* read = bitconv_read_string(vec, 0);
    printf("%s\n", read);
    free(read);

    for(size_t i = 0; i < bit_vector_size(vec); i++) {
        printf("%s", bit_vector_get(vec, i) ? "1":"0");
    }
    printf("\n");

    bit_vector_free(vec);
}
