#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include <stdbool.h>
#include "page_table.h"
#include "phys_mem.h"
#include "replacement.h"
#include "translate.h"
#include "vaddr_alloc.h"
#include "stats.h"

// agrupa todo el estado que necesita cada comando, para no pasar 5 parametros sueltos
// a cada funcion. main.c la arma una vez y la pasa por puntero.
typedef struct {
    page_table_t *pt;
    phys_mem_t *pm;
    replacement_policy_t *policy;
    vaddr_alloc_table_t *va;
    stats_t *stats;
} commands_ctx_t;

// parsea una linea completa del archivo de entrada (ej. "write 4096 99") y llama
// al cmd_* correspondiente. Lineas vacias o que empiezan con '#' se ignoran (true).
// devuelve false si el comando no existe o la linea esta mal formada.
bool commands_process_line(commands_ctx_t *ctx, const char *line);

// una funcion por comando, expuestas por si se quieren probar por separado
uint32_t cmd_alloc(commands_ctx_t *ctx, size_t bytes);
// el simulador no guarda contenido de memoria
// solo ejercitan la traduccion VA->PA para producir fallos, hits y estadisticas reales.
void cmd_write(commands_ctx_t *ctx, uint32_t vaddr, int value);
void cmd_read(commands_ctx_t *ctx, uint32_t vaddr);
bool cmd_free(commands_ctx_t *ctx, uint32_t vaddr);

#endif