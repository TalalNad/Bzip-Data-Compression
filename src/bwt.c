#include <stdlib.h>
#include "../include/bwt.h"

static const unsigned char *g_input = NULL;
static size_t g_len = 0;

int compare_rotations(const void *a, const void *b) {
    const RotationRef *ra = (const RotationRef *)a;
    const RotationRef *rb = (const RotationRef *)b;

    for (size_t k = 0; k < g_len; k++) {
        unsigned char ca = g_input[(ra->index + k) % g_len];
        unsigned char cb = g_input[(rb->index + k) % g_len];

        if (ca < cb) return -1;
        if (ca > cb) return 1;
    }
    return 0;
}

void bwt_encode(unsigned char *input, size_t len,
                unsigned char *output, int *primary_index) {
    if (len == 0) {
        *primary_index = 0;
        return;
    }

    RotationRef *rotations = (RotationRef *)malloc(len * sizeof(RotationRef));
    if (!rotations) {
        *primary_index = -1;
        return;
    }

    g_input = input;
    g_len = len;

    for (size_t i = 0; i < len; i++) {
        rotations[i].index = i;
    }

    qsort(rotations, len, sizeof(RotationRef), compare_rotations);

    for (size_t i = 0; i < len; i++) {
        size_t start = rotations[i].index;
        output[i] = input[(start + len - 1) % len];
        if (start == 0) {
            *primary_index = (int)i;
        }
    }

    free(rotations);
}

void bwt_decode(unsigned char *input, size_t len,
                int primary_index, unsigned char *output) {
    if (len == 0) {
        return;
    }

    int count[256] = {0};
    int start[256];
    int *occ = (int *)malloc(len * sizeof(int));
    int *next = (int *)malloc(len * sizeof(int));

    if (!occ || !next) {
        free(occ);
        free(next);
        return;
    }

    for (size_t i = 0; i < len; i++) {
        occ[i] = count[input[i]];
        count[input[i]]++;
    }

    int sum = 0;
    for (int c = 0; c < 256; c++) {
        start[c] = sum;
        sum += count[c];
    }

    for (size_t i = 0; i < len; i++) {
        next[i] = start[input[i]] + occ[i];
    }

    int idx = primary_index;
    for (int k = (int)len - 1; k >= 0; k--) {
        output[k] = input[idx];
        idx = next[idx];
    }

    free(occ);
    free(next);
}