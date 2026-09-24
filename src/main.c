#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "config.h"

static void print_usage(const char *prog) {
    fprintf(stderr, "Uso: %s <archivo_entrada> <fifo|lru> [memoria_fisica_kb] [tamano_pagina_bytes]\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }

    const char *input_path = argv[1];
    const char *policy_name = argv[2];

    size_t phys_mem_bytes = DEFAULT_PHYS_MEM_BYTES;
    if (argc >= 4) {
        long kb = strtol(argv[3], NULL, 10);
        if (kb <= 0) {
            fprintf(stderr, "memoria_fisica_kb invalida: %s\n", argv[3]);
            return 1;
        }
        phys_mem_bytes = (size_t)kb * 1024u;
    }

    // el tamano de pagina es una constante de compilacion (config.h), no se puede
    // cambiar en tiempo de ejecucion en esta version: si lo piden distinto, avisamos
    // y seguimos con PAGE_SIZE en vez de fallar silenciosamente.
    if (argc >= 5) {
        long requested_page_size = strtol(argv[4], NULL, 10);
        if (requested_page_size != (long)PAGE_SIZE) {
            fprintf(stderr,
                    "aviso: tamano_pagina_bytes=%ld ignorado, esta version usa PAGE_SIZE=%u fijo en config.h\n",
                    requested_page_size, (unsigned)PAGE_SIZE);
        }
    }

    replacement_policy_t *policy;
    int num_frames = (int)(phys_mem_bytes / PAGE_SIZE);
    if (strcmp(policy_name, "fifo") == 0) {
        policy = replacement_fifo_create(num_frames);
    } else if (strcmp(policy_name, "lru") == 0) {
        policy = replacement_lru_create(num_frames);
    } else {
        fprintf(stderr, "politica desconocida: %s (se esperaba 'fifo' o 'lru')\n", policy_name);
        print_usage(argv[0]);
        return 1;
    }

    FILE *f = fopen(input_path, "r");
    if (!f) {
        fprintf(stderr, "no se pudo abrir el archivo de entrada: %s\n", input_path);
        policy->destroy(policy->self);
        free(policy);
        return 1;
    }

    page_table_t *pt = page_table_create();
    phys_mem_t *pm = phys_mem_create(phys_mem_bytes);
    vaddr_alloc_table_t *va = vaddr_alloc_table_create();
    stats_t stats;
    stats_init(&stats);

    commands_ctx_t ctx = { pt, pm, policy, va, &stats };

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        commands_process_line(&ctx, line); // errores de una linea no detienen la simulacion completa
    }
    fclose(f);

    stats_print(&stats, policy->name);

    // liberar todo lo reservado, en orden inverso a como se creo (valgrind debe quedar limpio)
    vaddr_alloc_table_destroy(va);
    phys_mem_destroy(pm);
    page_table_destroy(pt);
    policy->destroy(policy->self);
    free(policy);

    return 0;
}