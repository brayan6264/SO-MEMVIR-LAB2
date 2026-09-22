#ifndef PAGE_FAULT_H
#define PAGE_FAULT_H

#include "page_table.h"
#include "phys_mem.h"
#include "replacement.h"
#include "stats.h"

// contrato del equipo, implementacion en src/page_fault.c (Persona B)
// devuelve el marco listo para usar; ya deja al PTE nuevo registrado en la politica (on_load)
int page_fault_handle(phys_mem_t *pm, replacement_policy_t *policy, pte_t *new_owner, stats_t *stats);

#endif
