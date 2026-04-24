#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

typedef struct {
    size_t block_size;
    int rle1_enabled;
    int bwt_enabled;
    char bwt_type[32];
    char input_directory[256];
    char output_directory[256];
} Config;

int load_config(const char *filename, Config *config);
void set_default_config(Config *config);

#endif