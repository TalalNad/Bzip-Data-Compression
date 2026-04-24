#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/archive.h"

int write_encoded_file(const char *filename, BlockManager *manager, const Config *config) {
    FILE *fp;
    FileHeader fh;

    if (!filename || !manager || !config) {
        return -1;
    }

    fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    memcpy(fh.magic, STAGE1_MAGIC, 4);
    fh.version = STAGE1_VERSION;
    fh.block_size = (uint32_t)config->block_size;
    fh.num_blocks = (uint32_t)manager->num_blocks;
    fh.flags = 0;
    if (config->rle1_enabled) {
        fh.flags |= 1u;
    }

    if (fwrite(&fh, sizeof(FileHeader), 1, fp) != 1) {
        fclose(fp);
        return -1;
    }

    for (int i = 0; i < manager->num_blocks; i++) {
        BlockHeader bh;
        Block *blk = &manager->blocks[i];

        bh.transformed_size = (uint32_t)blk->size;
        bh.original_size = (uint32_t)blk->original_size;
        bh.primary_index = blk->primary_index;

        if (fwrite(&bh, sizeof(BlockHeader), 1, fp) != 1) {
            fclose(fp);
            return -1;
        }

        if (blk->size > 0) {
            if (fwrite(blk->data, 1, blk->size, fp) != blk->size) {
                fclose(fp);
                return -1;
            }
        }
    }

    fclose(fp);
    return 0;
}

BlockManager *read_encoded_file(const char *filename, Config *config) {
    FILE *fp;
    FileHeader fh;
    BlockManager *manager;

    if (!filename || !config) {
        return NULL;
    }

    fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return NULL;
    }

    if (fread(&fh, sizeof(FileHeader), 1, fp) != 1) {
        fclose(fp);
        return NULL;
    }

    if (memcmp(fh.magic, STAGE1_MAGIC, 4) != 0) {
        fprintf(stderr, "Invalid file format.\n");
        fclose(fp);
        return NULL;
    }

    if (fh.version != STAGE1_VERSION) {
        fprintf(stderr, "Unsupported version: %u\n", fh.version);
        fclose(fp);
        return NULL;
    }

    config->block_size = fh.block_size;
    config->rle1_enabled = (fh.flags & 1u) ? 1 : 0;
    config->bwt_enabled = 1;
    strcpy(config->bwt_type, "matrix");

    manager = (BlockManager *)malloc(sizeof(BlockManager));
    if (!manager) {
        fclose(fp);
        return NULL;
    }

    manager->num_blocks = (int)fh.num_blocks;
    manager->block_size = fh.block_size;
    manager->blocks = (Block *)calloc(manager->num_blocks, sizeof(Block));

    if (!manager->blocks) {
        free(manager);
        fclose(fp);
        return NULL;
    }

    for (int i = 0; i < manager->num_blocks; i++) {
        BlockHeader bh;
        Block *blk = &manager->blocks[i];

        if (fread(&bh, sizeof(BlockHeader), 1, fp) != 1) {
            free_block_manager(manager);
            fclose(fp);
            return NULL;
        }

        blk->size = bh.transformed_size;
        blk->original_size = bh.original_size;
        blk->primary_index = bh.primary_index;

        if (blk->size > 0) {
            blk->data = (unsigned char *)malloc(blk->size);
            if (!blk->data) {
                free_block_manager(manager);
                fclose(fp);
                return NULL;
            }

            if (fread(blk->data, 1, blk->size, fp) != blk->size) {
                free_block_manager(manager);
                fclose(fp);
                return NULL;
            }
        } else {
            blk->data = NULL;
        }
    }

    fclose(fp);
    return manager;
}