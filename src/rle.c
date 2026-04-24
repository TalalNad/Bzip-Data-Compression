#include "../include/rle.h"

void rle1_encode(unsigned char *input, size_t len,
                 unsigned char *output, size_t *out_len) {
    size_t i = 0, j = 0;

    while (i < len) {
        unsigned char current = input[i];
        unsigned char count = 1;

        while ((i + count) < len && input[i + count] == current && count < 255) {
            count++;
        }

        output[j++] = count;
        output[j++] = current;
        i += count;
    }

    *out_len = j;
}

void rle1_decode(unsigned char *input, size_t len,
                 unsigned char *output, size_t *out_len) {
    size_t i = 0, j = 0;

    while (i + 1 < len) {
        unsigned char count = input[i++];
        unsigned char value = input[i++];

        for (unsigned int k = 0; k < count; k++) {
            output[j++] = value;
        }
    }

    *out_len = j;
}