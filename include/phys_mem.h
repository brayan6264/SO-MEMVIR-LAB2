#ifndef PHYS_MEM_H
#define PHYS_MEM_H

#include <stddef.h>
#include <stdbool.h>
#include "page_table.h"

// contrato del equipo, implementacion en src/phys_mem.c (Persona B)
typedef struct phys_mem phys_mem_t;

phys_mem_t *phys_mem_create(size_t phys_mem_bytes);
void phys_mem_destroy(phys_mem_t *pm);
bool phys_mem_has_free_frame(phys_mem_t *pm);
int phys_mem_alloc_frame(phys_mem_t *pm, pte_t *owner);
void phys_mem_free_frame(phys_mem_t *pm, int frame);
pte_t *phys_mem_owner(phys_mem_t *pm, int frame);

#endif
