#include "page_release.h"

static void page_release(phys_mem_t *pm, replacement_policy_t *policy, pte_t *pte) {
    policy->on_release(policy->self, (int)pte->frame);
    phys_mem_free_frame(pm, (int)pte->frame);
    pte->valid = false;
    pte->accessed = false;
    pte->dirty = false;
}

void page_release_range(page_table_t *pt, phys_mem_t *pm, replacement_policy_t *policy,
                        uint32_t base_vaddr, uint32_t size) {
    uint32_t page_size = page_table_config(pt)->page_size;
    for (uint32_t offset = 0; offset < size; offset += page_size) {
        pte_t *pte = page_table_get_pte(pt, base_vaddr + offset, false);
        if (pte && pte->valid) {
            page_release(pm, policy, pte);
        }
    }
}
