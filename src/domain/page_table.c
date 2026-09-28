#include <stdio.h>
#include <stdlib.h>
#include "domain/page_table.h"

struct page_table {
    page_config_t cfg;
    pte_t **pt2; // NULL hasta que se crea bajo demanda
};

static void *checked_calloc(size_t count, size_t size) {
    void *memory = calloc(count, size);
    if (!memory) {
        fprintf(stderr, "page_table: sin memoria\n");
        exit(1);
    }
    return memory;
}

page_table_t *page_table_create(const page_config_t *cfg) {
    page_table_t *pt = checked_calloc(1, sizeof(page_table_t));
    pt->cfg = *cfg;
    pt->pt2 = checked_calloc(cfg->pt1_entries, sizeof(pte_t *));
    return pt;
}

void page_table_destroy(page_table_t *pt) {
    if (!pt) return;
    for (uint32_t i = 0; i < pt->cfg.pt1_entries; i++) {
        free(pt->pt2[i]);
    }
    free(pt->pt2);
    free(pt);
}

const page_config_t *page_table_config(const page_table_t *pt) {
    return &pt->cfg;
}

pte_t *page_table_get_pte(page_table_t *pt, uint32_t vaddr, bool create) {
    uint32_t idx1 = page_config_pt1_index(&pt->cfg, vaddr);
    uint32_t idx2 = page_config_pt2_index(&pt->cfg, vaddr);

    if (pt->pt2[idx1] == NULL) {
        if (!create) return NULL;
        pt->pt2[idx1] = checked_calloc(pt->cfg.pt2_entries, sizeof(pte_t));
    }
    return &pt->pt2[idx1][idx2];
}
