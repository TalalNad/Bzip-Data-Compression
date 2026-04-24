#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <stdint.h>
#include "block.h"
#include "config.h"

#define STAGE1_MAGIC "BZP1"
#define STAGE1_VERSION 1

typedef struct {
    char magic[4];          /* "BZP1" */
    uint32_t version;       /* 1 */
    uint32_t block_size;    /* config block size */
    uint32_t num_blocks;    /* number of blocks */
    uint32_t flags;         /* bit 0 = rle1 enabled */
} FileHeader;

typedef struct {
    uint32_t transformed_size; /* bytes stored for this block */
    uint32_t original_size;    /* bytes before Stage 1 encode */
    int32_t primary_index;     /* needed for inverse BWT */
} BlockHeader;

int write_encoded_file(const char *filename, BlockManager *manager, const Config *config);
BlockManager *read_encoded_file(const char *filename, Config *config);

#endif