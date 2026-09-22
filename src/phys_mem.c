#include <stdio.h>
#include <stdlib.h>
#include "phys_mem.h"
#include "config.h"

struct phys_mem {
    int num_frames;
    int *free_stack;
    int top; // -1 cuando no hay marcos libres
    pte_t **owner;
};

phys_mem_t *phys_mem_create(size_t phys_mem_bytes) {
    phys_mem_t *pm = calloc(1, sizeof(phys_mem_t));
    if (!pm) {
        fprintf(stderr, "phys_mem_create: sin memoria\n");
        exit(1);
    }

    pm->num_frames = (int)(phys_mem_bytes / PAGE_SIZE);
    pm->free_stack = malloc(sizeof(int) * (size_t)pm->num_frames);
    pm->owner = calloc((size_t)pm->num_frames, sizeof(pte_t *));
    if (!pm->free_stack || !pm->owner) {
        fprintf(stderr, "phys_mem_create: sin memoria\n");
        exit(1);
    }

    for (int i = 0; i < pm->num_frames; i++) {
        pm->free_stack[i] = pm->num_frames - 1 - i; // entrega marcos en orden: 0, 1, 2...
    }
    pm->top = pm->num_frames - 1;
    return pm;
}

void phys_mem_destroy(phys_mem_t *pm) {
    if (!pm) return;
    free(pm->free_stack);
    free(pm->owner);
    free(pm);
}

bool phys_mem_has_free_frame(phys_mem_t *pm) {
    return pm->top >= 0;
}

int phys_mem_alloc_frame(phys_mem_t *pm, pte_t *owner) {
    int frame = pm->free_stack[pm->top--];
    pm->owner[frame] = owner;
    return frame;
}

void phys_mem_free_frame(phys_mem_t *pm, int frame) {
    pm->owner[frame] = NULL;
    pm->free_stack[++pm->top] = frame;
}

pte_t *phys_mem_owner(phys_mem_t *pm, int frame) {
    return pm->owner[frame];
}
