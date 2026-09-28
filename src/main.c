#include <stdio.h>
#include "infrastructure/cli_options.h"
#include "application/commands.h"
#include "infrastructure/replacement_factory.h"

static void run_simulation(FILE *input, commands_ctx_t *ctx) {
    char line[256];
    clock_t start = clock();
    while (fgets(line, sizeof(line), input)) {
        commands_process_line(ctx, line); // errores de una linea no detienen la simulacion completa
    }
    ctx->stats->total_ms = stats_elapsed_ms(start);
}

int main(int argc, char *argv[]) {
    cli_options_t opts;
    if (!cli_options_parse(argc, argv, &opts)) {
        return 1;
    }

    int num_frames = (int)(opts.phys_mem_bytes / opts.page_cfg.page_size);
    replacement_policy_t *policy = replacement_create(opts.policy_name, num_frames);
    if (!policy) {
        fprintf(stderr, "politica desconocida: %s (se esperaba 'fifo' o 'lru')\n", opts.policy_name);
        return 1;
    }

    FILE *input = fopen(opts.input_path, "r");
    if (!input) {
        fprintf(stderr, "no se pudo abrir el archivo de entrada: %s\n", opts.input_path);
        replacement_destroy(policy);
        return 1;
    }

    page_table_t *pt = page_table_create(&opts.page_cfg);
    phys_mem_t *pm = phys_mem_create(opts.phys_mem_bytes, opts.page_cfg.page_size);
    vaddr_alloc_table_t *va = vaddr_alloc_table_create(opts.page_cfg.page_size);
    stats_t stats;
    stats_init(&stats);
    commands_ctx_t ctx = { pt, pm, policy, va, &stats };

    run_simulation(input, &ctx);
    fclose(input);
    stats_print(&stats, policy->name);

    vaddr_alloc_table_destroy(va);
    phys_mem_destroy(pm);
    page_table_destroy(pt);
    replacement_destroy(policy);
    return 0;
}
