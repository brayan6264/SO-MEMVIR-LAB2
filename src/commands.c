
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

uint32_t cmd_alloc(commands_ctx_t *ctx, size_t bytes) {
    return vaddr_alloc_table_alloc(ctx->va, bytes);
}

void cmd_write(commands_ctx_t *ctx, uint32_t vaddr, int value) {
    (void)value; // no se guarda contenido: solo interesa que dispare la traduccion
    translate(ctx->pt, ctx->pm, ctx->policy, ctx->stats, vaddr, true);
}

void cmd_read(commands_ctx_t *ctx, uint32_t vaddr) {
    translate(ctx->pt, ctx->pm, ctx->policy, ctx->stats, vaddr, false);
}

bool cmd_free(commands_ctx_t *ctx, uint32_t vaddr) {
    bool ok = vaddr_alloc_table_free(ctx->va, vaddr, ctx->pt, ctx->pm);
    if (!ok) {
        fprintf(stderr, "free: 0x%x no es la base de ningun alloc vivo, se ignora\n", vaddr);
    }
    return ok;
}

bool commands_process_line(commands_ctx_t *ctx, const char *line) {
    char cmd[16];

    if (sscanf(line, "%15s", cmd) != 1) {
        return true; // linea vacia o solo espacios en blanco: se ignora sin error
    }
    if (cmd[0] == '#') {
        return true; // comentario
    }

    if (strcmp(cmd, "alloc") == 0) {
        unsigned long bytes;
        if (sscanf(line, "%15s %lu", cmd, &bytes) != 2 || bytes == 0) {
            fprintf(stderr, "linea invalida (alloc): %s\n", line);
            return false;
        }
        cmd_alloc(ctx, (size_t)bytes);
        return true;
    }

    if (strcmp(cmd, "write") == 0) {
        unsigned long vaddr;
        int value;
        if (sscanf(line, "%15s %lu %d", cmd, &vaddr, &value) != 3) {
            fprintf(stderr, "linea invalida (write): %s\n", line);
            return false;
        }
        cmd_write(ctx, (uint32_t)vaddr, value);
        return true;
    }

    if (strcmp(cmd, "read") == 0) {
        unsigned long vaddr;
        if (sscanf(line, "%15s %lu", cmd, &vaddr) != 2) {
            fprintf(stderr, "linea invalida (read): %s\n", line);
            return false;
        }
        cmd_read(ctx, (uint32_t)vaddr);
        return true;
    }

    if (strcmp(cmd, "free") == 0) {
        unsigned long vaddr;
        if (sscanf(line, "%15s %lu", cmd, &vaddr) != 2) {
            fprintf(stderr, "linea invalida (free): %s\n", line);
            return false;
        }
        cmd_free(ctx, (uint32_t)vaddr);
        return true;
    }

    fprintf(stderr, "comando desconocido: %s\n", cmd);
    return false;
}