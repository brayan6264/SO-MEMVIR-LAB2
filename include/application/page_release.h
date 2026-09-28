#ifndef PAGE_RELEASE_H
#define PAGE_RELEASE_H

#include <stdint.h>
#include "domain/page_table.h"
#include "domain/phys_mem.h"
#include "domain/replacement.h"

void page_release_range(page_table_t *pt, phys_mem_t *pm, replacement_policy_t *policy,
                        uint32_t base_vaddr, uint32_t size);

#endif
