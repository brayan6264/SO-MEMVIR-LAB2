#ifndef VADDR_ALLOC_H
#define VADDR_ALLOC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "page_table.h"
#include "phys_mem.h"

// contrato del equipo, implementacion en src/vaddr_alloc.c (paso 3)
typedef struct vaddr_alloc_table vaddr_alloc_table_t;

vaddr_alloc_table_t *vaddr_alloc_table_create(void);
void vaddr_alloc_table_destroy(vaddr_alloc_table_t *t);
// bytes se redondea hacia arriba al tamano de pagina; devuelve la VA base del bloque
uint32_t vaddr_alloc_table_alloc(vaddr_alloc_table_t *t, size_t bytes);
// base_vaddr debe ser una VA devuelta antes por _alloc (no cualquier direccion dentro
// del bloque); libera sus frames fisicos y quita las PTE asociadas. false si base_vaddr
// no corresponde a ningun alloc vivo.
bool vaddr_alloc_table_free(vaddr_alloc_table_t *t, uint32_t base_vaddr,
                             page_table_t *pt, phys_mem_t *pm);

#endif