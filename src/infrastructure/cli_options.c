#include <stdio.h>
#include <stdlib.h>
#include "infrastructure/cli_options.h"
#include "domain/config.h"

static void print_usage(const char *prog) {
    fprintf(stderr, "Uso: %s <archivo_entrada> <fifo|lru> [memoria_fisica_kb] [tamano_pagina_bytes]\n", prog);
}

static bool parse_phys_mem(const char *arg, size_t *bytes) {
    long kb = strtol(arg, NULL, 10);
    if (kb < MIN_PHYS_MEM_KB) {
        fprintf(stderr, "memoria_fisica_kb invalida: %s (minimo %d KB)\n", arg, MIN_PHYS_MEM_KB);
        return false;
    }
    *bytes = (size_t)kb * 1024u;
    return true;
}

static bool parse_page_size(const char *arg, page_config_t *cfg) {
    long page_size = strtol(arg, NULL, 10);
    bool in_range = page_size >= (long)MIN_PAGE_SIZE && page_size <= (long)MAX_PAGE_SIZE;
    if (!in_range || !page_config_init(cfg, (uint32_t)page_size)) {
        fprintf(stderr, "tamano_pagina_bytes invalido: %s (potencia de 2 entre %u y %u)\n",
                arg, MIN_PAGE_SIZE, MAX_PAGE_SIZE);
        return false;
    }
    return true;
}

bool cli_options_parse(int argc, char *argv[], cli_options_t *opts) {
    if (argc < 3) {
        print_usage(argv[0]);
        return false;
    }
    opts->input_path = argv[1];
    opts->policy_name = argv[2];
    opts->phys_mem_bytes = DEFAULT_PHYS_MEM_BYTES;
    page_config_init(&opts->page_cfg, DEFAULT_PAGE_SIZE);

    if (argc >= 4 && !parse_phys_mem(argv[3], &opts->phys_mem_bytes)) return false;
    if (argc >= 5 && !parse_page_size(argv[4], &opts->page_cfg)) return false;
    return true;
}
