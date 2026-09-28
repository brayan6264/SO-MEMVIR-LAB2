#include "page_fault.h"

static int evict_victim_frame(phys_mem_t *pm, replacement_policy_t *policy, pte_t *new_owner,
                              stats_t *stats) {
    int frame = policy->select_victim(policy->self);
    pte_t *victim_pte = phys_mem_owner(pm, frame);
    victim_pte->valid = false; // saca la pagina vieja de memoria
    phys_mem_free_frame(pm, frame);
    stats->replacements++;
    return phys_mem_alloc_frame(pm, new_owner); // recupera el mismo marco ya libre
}

static int obtain_frame(phys_mem_t *pm, replacement_policy_t *policy, pte_t *new_owner,
                        stats_t *stats) {
    if (phys_mem_has_free_frame(pm)) {
        return phys_mem_alloc_frame(pm, new_owner);
    }
    return evict_victim_frame(pm, policy, new_owner, stats);
}

int page_fault_handle(phys_mem_t *pm, replacement_policy_t *policy, pte_t *new_owner, stats_t *stats) {
    clock_t start = clock();
    stats->faults++;

    int frame = obtain_frame(pm, policy, new_owner, stats);
    policy->on_load(policy->self, frame);

    stats->fault_ms += stats_elapsed_ms(start);
    return frame;
}
