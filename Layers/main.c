#include "bitconv.h"
#include "error_check.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

static bool colorNonPrintable;
static const error_check_mode* errorCheck;

static void sender_application();
static void sender_application_layer(char* message);
static void sender_link_layer(bit_vector* vec);
static void comm_layer(bit_vector* vec);
static void receiver_link_layer(bit_vector* vec);
static void receiver_application_layer(bit_vector* vec);
static void receiver_application(char* message);

static void print_bits(const bit_vector* vec) {
    size_t remaining_bits = bit_vector_size(vec);
    size_t pos = 0;
    size_t bytes_line = 0;
    while(remaining_bits >= 8) {
        if(++bytes_line == 8) {
            bytes_line = 0;
            printf("\n");
        }
        for(size_t i = 0; i < 8; i++) {
            printf("%s", bit_vector_get(vec, pos++) ? "1" : "0");
        }
        printf(" ");
        remaining_bits -= 8;
    }
    if(++bytes_line == 8) {
        printf("\n");
    }
    if(remaining_bits > 0) {
        while(remaining_bits > 0) {
            printf("%s", bit_vector_get(vec, pos++) ? "1" : "0");
            remaining_bits--;
        }
        printf("\n");
    }
}

int main(void) {
    colorNonPrintable = getenv("NO_COLOR") == NULL;
    char* mode = getenv("ERROR_CHECK_MODE");
    if(mode) {
        if(!strcmp(mode, "crc32")) {
            errorCheck = ERROR_CHECK_CRC32;
        } else if(!strcmp(mode, "parity_even")) {
            errorCheck = ERROR_CHECK_PARITY_EVEN;
        } else if(!strcmp(mode, "parity_odd")) {
            errorCheck = ERROR_CHECK_PARITY_ODD;
        } else {
            fprintf(stderr, "Unknown mode '%s', must be one of crc32, parity_even or parity_odd", mode);
            fflush(stderr);
            exit(1);
        }
    } else {
        errorCheck = ERROR_CHECK_CRC32;
    }
    printf("Using error check mode \x1b[31m%s\x1b[0m\n", error_check_name(errorCheck));
    if(colorNonPrintable) {
        printf("Non printable characters on the receiving application will\n");
        printf("be printed in hexadecimal and \x1b[31mred\x1b[0m\n");
    }
    sender_application();
}

static void sender_application() {
    const int size = 1000;

    char* buf = calloc(sizeof(*buf), size);
    printf("Type a message: ");
    fflush(stdout);
    fgets(buf, size, stdin);

    size_t len = strlen(buf);
    if(len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';

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
    printf("sender_link_layer:\n");
    print_bits(vec);
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
    printf("receiver_link_layer:\n");
    print_bits(vec);
    if(!error_check_validate(errorCheck, vec)) {
        fprintf(stderr, "\x1b[31mError check failed, message is likely corrupted\x1b[0m\n");
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
    printf("Received message '");
    if(colorNonPrintable) {
        size_t len = strlen(message);
        for(size_t i = 0; i < len; i++) {
            char ch = message[i];
            if(isprint(ch)) {
                fputc(message[i], stdout);
            } else {
                printf("\x1b[31m0x%2X\x1b[0m", (uint8_t)ch);
            }
        }
    } else {
        printf("%s", message);
    }
    printf("'\n");
    free(message);
}
