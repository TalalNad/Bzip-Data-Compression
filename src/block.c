#include <stdio.h>
#include <stdlib.h>
#include "../include/block.h"

BlockManager *divide_into_blocks(const char *filename, size_t block_size) {
    FILE *fp = fopen(filename, "rb");
    long file_size;
    int num_blocks;
    BlockManager *manager;

    if (!fp) {
        perror("fopen");
        return NULL;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }

    file_size = ftell(fp);
    if (file_size < 0) {
        fclose(fp);
        return NULL;
    }

    rewind(fp);

    num_blocks = (file_size == 0) ? 0 : (int)((file_size + (long)block_size - 1) / (long)block_size);

    manager = (BlockManager *)malloc(sizeof(BlockManager));
    if (!manager) {
        fclose(fp);
        return NULL;
    }

    manager->num_blocks = num_blocks;
    manager->block_size = block_size;
    manager->blocks = (num_blocks > 0) ? (Block *)calloc(num_blocks, sizeof(Block)) : NULL;

    if (num_blocks > 0 && !manager->blocks) {
        free(manager);
        fclose(fp);
        return NULL;
    }

    for (int i = 0; i < num_blocks; i++) {
        size_t bytes_to_read = block_size;
        size_t bytes_read;

        if ((long)((i + 1) * block_size) > file_size) {
            bytes_to_read = (size_t)(file_size - (long)i * (long)block_size);
        }

        manager->blocks[i].data = (unsigned char *)malloc(bytes_to_read);
        if (!manager->blocks[i].data) {
            free_block_manager(manager);
            fclose(fp);
            return NULL;
        }

        bytes_read = fread(manager->blocks[i].data, 1, bytes_to_read, fp);
        if (bytes_read != bytes_to_read) {
            free_block_manager(manager);
            fclose(fp);
            return NULL;
        }

        manager->blocks[i].size = bytes_read;
        manager->blocks[i].original_size = bytes_read;
        manager->blocks[i].primary_index = -1;
    }

    fclose(fp);
    return manager;
}

int reassemble_blocks(BlockManager *manager, const char *output_filename) {
    FILE *fp;

    if (!manager || !output_filename) {
        return -1;
    }

    fp = fopen(output_filename, "wb");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    for (int i = 0; i < manager->num_blocks; i++) {
        if (manager->blocks[i].size > 0) {
            if (fwrite(manager->blocks[i].data, 1, manager->blocks[i].size, fp) != manager->blocks[i].size) {
                fclose(fp);
                return -1;
            }
        }
    }

    fclose(fp);
    return 0;
}

void free_block_manager(BlockManager *manager) {
    if (!manager) {
        return;
    }

    if (manager->blocks) {
        for (int i = 0; i < manager->num_blocks; i++) {
            free(manager->blocks[i].data);
        }
        free(manager->blocks);
    }

    free(manager);
}