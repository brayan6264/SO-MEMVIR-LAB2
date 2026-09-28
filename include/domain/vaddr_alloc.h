#ifndef VADDR_ALLOC_H
#define VADDR_ALLOC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// contrato del equipo, implementacion en src/vaddr_alloc.c (paso 3)
typedef struct vaddr_alloc_table vaddr_alloc_table_t;

vaddr_alloc_table_t *vaddr_alloc_table_create(uint32_t page_size);
void vaddr_alloc_table_destroy(vaddr_alloc_table_t *t);
// bytes se redondea hacia arriba al tamano de pagina; devuelve la VA base del bloque
uint32_t vaddr_alloc_table_alloc(vaddr_alloc_table_t *t, size_t bytes);
// devuelve en size_out el tamano del bloque que empezaba exactamente en base_vaddr
bool vaddr_alloc_table_free(vaddr_alloc_table_t *t, uint32_t base_vaddr, uint32_t *size_out);

#endif