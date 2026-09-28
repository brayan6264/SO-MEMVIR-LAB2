#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <stdint.h>
#include <stdbool.h>
#include "domain/page_table.h"
#include "domain/phys_mem.h"
#include "domain/replacement.h"
#include "domain/stats.h"

uint32_t translate(page_table_t *pt, phys_mem_t *pm, replacement_policy_t *policy,
                    stats_t *stats, uint32_t vaddr, bool is_write);

#endif
