#include "translate.h"
#include "config.h"
#include "page_fault.h"

uint32_t translate(page_table_t *pt, phys_mem_t *pm, replacement_policy_t *policy,
                    stats_t *stats, uint32_t vaddr, bool is_write) {
    uint32_t offset = vaddr & (PAGE_SIZE - 1);
    pte_t *pte = page_table_get_pte(pt, vaddr, true);

    stats->accesses++;

    if (pte->valid) {
        policy->on_access(policy->self, (int)pte->frame);
    } else {
        // page_fault_handle ya avisa a la politica (on_load) y actualiza stats->faults/replacements
        int frame = page_fault_handle(pm, policy, pte, stats);
        pte->frame = (uint32_t)frame;
        pte->valid = true;
    }

    pte->accessed = true;
    if (is_write) pte->dirty = true;

    return pte->frame * PAGE_SIZE + offset;
}
