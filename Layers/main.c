#include "bitconv.h"
#include "error_check.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const error_check_mode* errorCheck;

static void sender_application();
static void sender_application_layer(char* message);
static void sender_link_layer(bit_vector* vec);
static void comm_layer(bit_vector* vec);
static void receiver_link_layer(bit_vector* vec);
static void receiver_application_layer(bit_vector* vec);
static void receiver_application(char* message);

int main(void) {
    errorCheck = ERROR_CHECK_PARITY_EVEN;
    sender_application();
}

static void sender_application() {
    const int size = 1000;

    char* buf = malloc(sizeof(*buf) * size);
    printf("Type a message: ");
    fflush(stdout);
    fgets(buf, size, stdin);

    size_t len = strlen(buf);
    if(buf[len - 1] == '\n') buf[len - 1] = '\0';

    sender_application_layer(buf);
}

static void sender_application_layer(char* message) {
    bit_vector* vec = bit_vector_new();
    bitconv_push_string(vec, message);
    free(message);
    sender_link_layer(vec);
}

static void sender_link_layer(bit_vector* vec) {
    error_check_compute(errorCheck, vec);
    comm_layer(vec);
}

static void comm_layer(bit_vector* vec) {
    const int error_rate = 2;

    srand(time(NULL)); // NOLINT(cert-msc51-cpp)
    for(size_t i = 0; i < bit_vector_size(vec); i++) {
        if(rand() % 100 < error_rate) {  // NOLINT(cert-msc50-cpp)
            //bit flip
            bit_vector_set(vec, i, !bit_vector_get(vec, i));
        }
    }
    receiver_link_layer(vec);
}

static void receiver_link_layer(bit_vector* vec) {
    if(!error_check_validate(errorCheck, vec)) {
        fprintf(stderr, "Error check failed, message is likely corrupted\n");
        fflush(stderr);
    }
    //strip error code from data
    size_t error_check_bits = error_check_bit_size(errorCheck);
    bit_vector_remove_range(vec, bit_vector_size(vec) - error_check_bits, error_check_bits);

    receiver_application_layer(vec);
}

static void receiver_application_layer (bit_vector* vec) {
    char* msg = bitconv_read_string(vec, 0);
    bit_vector_free(vec);
    receiver_application(msg);
}

static void receiver_application(char* message) {
    printf("Received message '%s'\n", message);
    free(message);
}
