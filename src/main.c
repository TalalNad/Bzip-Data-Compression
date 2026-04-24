#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/config.h"
#include "../include/block.h"
#include "../include/rle.h"
#include "../include/bwt.h"
#include "../include/archive.h"

static int encode_stage1(const char *input_file, const char *output_file, Config *config) {
    BlockManager *manager = divide_into_blocks(input_file, config->block_size);
    if (!manager) {
        fprintf(stderr, "Failed to divide input into blocks.\n");
        return -1;
    }

    for (int i = 0; i < manager->num_blocks; i++) {
        Block *blk = &manager->blocks[i];
        unsigned char *rle_out = NULL;
        unsigned char *bwt_out = NULL;
        size_t rle_len = blk->size;

        if (config->rle1_enabled) {
            rle_out = (unsigned char *)malloc(blk->size * 2 + 2);
            if (!rle_out) {
                free_block_manager(manager);
                return -1;
            }

            rle1_encode(blk->data, blk->size, rle_out, &rle_len);
        } else {
            rle_out = (unsigned char *)malloc(blk->size);
            if (!rle_out) {
                free_block_manager(manager);
                return -1;
            }
            memcpy(rle_out, blk->data, blk->size);
        }

        bwt_out = (unsigned char *)malloc(rle_len);
        if (!bwt_out) {
            free(rle_out);
            free_block_manager(manager);
            return -1;
        }

        bwt_encode(rle_out, rle_len, bwt_out, &blk->primary_index);

        free(blk->data);
        free(rle_out);

        blk->data = bwt_out;
        blk->size = rle_len;
    }

    if (write_encoded_file(output_file, manager, config) != 0) {
        fprintf(stderr, "Failed to write encoded file.\n");
        free_block_manager(manager);
        return -1;
    }

    free_block_manager(manager);
    return 0;
}

static int decode_stage1(const char *input_file, const char *output_file, Config *config) {
    BlockManager *manager = read_encoded_file(input_file, config);
    if (!manager) {
        fprintf(stderr, "Failed to read encoded file.\n");
        return -1;
    }

    for (int i = 0; i < manager->num_blocks; i++) {
        Block *blk = &manager->blocks[i];
        unsigned char *bwt_decoded = NULL;
        unsigned char *final_out = NULL;
        size_t final_len = 0;

        bwt_decoded = (unsigned char *)malloc(blk->size);
        if (!bwt_decoded) {
            free_block_manager(manager);
            return -1;
        }

        bwt_decode(blk->data, blk->size, blk->primary_index, bwt_decoded);

        if (config->rle1_enabled) {
            final_out = (unsigned char *)malloc(blk->original_size);
            if (!final_out) {
                free(bwt_decoded);
                free_block_manager(manager);
                return -1;
            }

            rle1_decode(bwt_decoded, blk->size, final_out, &final_len);
            free(bwt_decoded);

            if (final_len != blk->original_size) {
                fprintf(stderr, "Decoded block size mismatch on block %d.\n", i);
                free(final_out);
                free_block_manager(manager);
                return -1;
            }
        } else {
            final_out = bwt_decoded;
            final_len = blk->size;
        }

        free(blk->data);
        blk->data = final_out;
        blk->size = final_len;
    }

    if (reassemble_blocks(manager, output_file) != 0) {
        fprintf(stderr, "Failed to reassemble decoded blocks.\n");
        free_block_manager(manager);
        return -1;
    }

    free_block_manager(manager);
    return 0;
}

int main(int argc, char *argv[]) {
    Config config;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <encode|decode> <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    if (load_config("config.ini", &config) != 0) {
        fprintf(stderr, "Failed to load config.ini\n");
        return 1;
    }

    if (strcmp(argv[1], "encode") == 0) {
        return encode_stage1(argv[2], argv[3], &config) == 0 ? 0 : 1;
    } else if (strcmp(argv[1], "decode") == 0) {
        return decode_stage1(argv[2], argv[3], &config) == 0 ? 0 : 1;
    } else {
        fprintf(stderr, "Invalid mode: %s\n", argv[1]);
        return 1;
    }
}