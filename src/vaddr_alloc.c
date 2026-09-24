#include <stdio.h>
#include <stdlib.h>
#include "vaddr_alloc.h"
#include "config.h"

// lista simple de bloques reservados: {base, size} + siguiente
typedef struct alloc_node {
    uint32_t base;
    uint32_t size;
    struct alloc_node *next;
} alloc_node_t;

struct vaddr_alloc_table {
    uint32_t next_free;      // puntero que avanza: siguiente VA disponible para un alloc nuevo
    alloc_node_t *allocations; // bloques actualmente reservados (para que free los reconozca)
};

vaddr_alloc_table_t *vaddr_alloc_table_create(void) {
    vaddr_alloc_table_t *t = calloc(1, sizeof(vaddr_alloc_table_t));
    if (!t) {
        fprintf(stderr, "vaddr_alloc_table_create: sin memoria\n");
        exit(1);
    }
    t->next_free = 0;
    t->allocations = NULL;
    return t;
}

void vaddr_alloc_table_destroy(vaddr_alloc_table_t *t) {
    if (!t) return;
    alloc_node_t *node = t->allocations;
    while (node) {
        alloc_node_t *next = node->next;
        free(node);
        node = next;
    }
    free(t);
}

uint32_t vaddr_alloc_table_alloc(vaddr_alloc_table_t *t, size_t bytes) {
    // redondea hacia arriba al tamano de pagina (todo alloc ocupa un numero entero de paginas)
    uint32_t pages = (uint32_t)((bytes + PAGE_SIZE - 1) / PAGE_SIZE);
    uint32_t size = pages * PAGE_SIZE;

    uint32_t base = t->next_free;
    t->next_free += size;

    alloc_node_t *node = calloc(1, sizeof(alloc_node_t));
    if (!node) {
        fprintf(stderr, "vaddr_alloc_table_alloc: sin memoria\n");
        exit(1);
    }
    node->base = base;
    node->size = size;
    node->next = t->allocations;
    t->allocations = node;

    return base;
}

bool vaddr_alloc_table_free(vaddr_alloc_table_t *t, uint32_t base_vaddr,
                             page_table_t *pt, phys_mem_t *pm) {
    // busca el bloque que empieza exactamente en base_vaddr
    alloc_node_t **link = &t->allocations;
    while (*link && (*link)->base != base_vaddr) {
        link = &(*link)->next;
    }
    if (!*link) return false; // no hay ningun alloc vivo con esa base

    alloc_node_t *node = *link;

    // libera cada pagina del bloque que efectivamente llego a cargarse en memoria fisica
    for (uint32_t off = 0; off < node->size; off += PAGE_SIZE) {
        pte_t *pte = page_table_get_pte(pt, node->base + off, false);
        if (pte && pte->valid) {
            phys_mem_free_frame(pm, (int)pte->frame);
            pte->valid = false;
            pte->accessed = false;
            pte->dirty = false;
        }
    }

    *link = node->next;
    free(node);
    return true;
}