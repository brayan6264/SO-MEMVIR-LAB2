#include "page_fault.h"

int page_fault_handle(phys_mem_t *pm, replacement_policy_t *policy, pte_t *new_owner, stats_t *stats) {
    stats->faults++;

    int frame;
    if (phys_mem_has_free_frame(pm)) {
        frame = phys_mem_alloc_frame(pm, new_owner);
    } else {
        frame = policy->select_victim(policy->self);
        pte_t *victim_pte = phys_mem_owner(pm, frame);
        victim_pte->valid = false; // saca la pagina vieja de memoria
        phys_mem_free_frame(pm, frame);
        frame = phys_mem_alloc_frame(pm, new_owner); // recupera el mismo marco ya libre
        stats->replacements++;
    }

    policy->on_load(policy->self, frame);
    return frame;
}
