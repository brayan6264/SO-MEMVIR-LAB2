#ifndef CLI_OPTIONS_H
#define CLI_OPTIONS_H

#include <stddef.h>
#include <stdbool.h>
#include "page_config.h"

typedef struct {
    const char *input_path;
    const char *policy_name;
    size_t phys_mem_bytes;
    page_config_t page_cfg;
} cli_options_t;

bool cli_options_parse(int argc, char *argv[], cli_options_t *opts);

#endif
