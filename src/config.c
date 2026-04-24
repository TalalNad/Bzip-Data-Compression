#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/config.h"

static void trim(char *str) {
    char *start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}

void set_default_config(Config *config) {
    config->block_size = 500000;
    config->rle1_enabled = 1;
    config->bwt_enabled = 1;
    strcpy(config->bwt_type, "matrix");
    strcpy(config->input_directory, "./benchmarks/");
    strcpy(config->output_directory, "./results/");
}

static int parse_bool(const char *value) {
    return (strcmp(value, "true") == 0 || strcmp(value, "1") == 0) ? 1 : 0;
}

int load_config(const char *filename, Config *config) {
    FILE *fp = fopen(filename, "r");
    char line[512];

    if (!fp) {
        return -1;
    }

    set_default_config(config);

    while (fgets(line, sizeof(line), fp)) {
        char *eq;
        char *comment;

        trim(line);

        if (line[0] == '\0' || line[0] == '#' || line[0] == ';' || line[0] == '[') {
            continue;
        }

        comment = strchr(line, '#');
        if (comment) {
            *comment = '\0';
            trim(line);
        }

        eq = strchr(line, '=');
        if (!eq) {
            continue;
        }

        *eq = '\0';

        char key[256], value[256];
        strncpy(key, line, sizeof(key) - 1);
        key[sizeof(key) - 1] = '\0';
        strncpy(value, eq + 1, sizeof(value) - 1);
        value[sizeof(value) - 1] = '\0';

        trim(key);
        trim(value);

        if (strcmp(key, "block_size") == 0) {
            config->block_size = (size_t)strtoul(value, NULL, 10);
        } else if (strcmp(key, "rle1_enabled") == 0) {
            config->rle1_enabled = parse_bool(value);
        } else if (strcmp(key, "bwt_type") == 0) {
            strncpy(config->bwt_type, value, sizeof(config->bwt_type) - 1);
            config->bwt_type[sizeof(config->bwt_type) - 1] = '\0';
        } else if (strcmp(key, "input_directory") == 0) {
            strncpy(config->input_directory, value, sizeof(config->input_directory) - 1);
            config->input_directory[sizeof(config->input_directory) - 1] = '\0';
        } else if (strcmp(key, "output_directory") == 0) {
            strncpy(config->output_directory, value, sizeof(config->output_directory) - 1);
            config->output_directory[sizeof(config->output_directory) - 1] = '\0';
        }
    }

    fclose(fp);

    if (config->block_size < 100000 || config->block_size > 900000) {
        fprintf(stderr, "Error: block_size must be between 100000 and 900000 bytes.\n");
        return -1;
    }

    if (strcmp(config->bwt_type, "matrix") != 0) {
        fprintf(stderr, "Error: only matrix BWT is supported in Phase 1.\n");
        return -1;
    }

    return 0;
}