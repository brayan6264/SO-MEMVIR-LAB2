#include <stdio.h>
#include <stdlib.h>
#include "page_table.h"
#include "config.h"

struct page_table {
    pte_t *pt2[PT_ENTRIES]; // NULL hasta que se crea bajo demanda
};

page_table_t *page_table_create(void) {
    page_table_t *pt = calloc(1, sizeof(page_table_t));
    if (!pt) {
        fprintf(stderr, "page_table_create: sin memoria\n");
        exit(1);
    }
    return pt;
}

void page_table_destroy(page_table_t *pt) {
    if (!pt) return;
    for (int i = 0; i < PT_ENTRIES; i++) {
        free(pt->pt2[i]);
    }
    free(pt);
}

pte_t *page_table_get_pte(page_table_t *pt, uint32_t vaddr, bool create) {
    uint32_t idx1 = (vaddr >> (PT2_BITS + OFFSET_BITS)) & (PT_ENTRIES - 1);
    uint32_t idx2 = (vaddr >> OFFSET_BITS) & (PT_ENTRIES - 1);

    if (pt->pt2[idx1] == NULL) {
        if (!create) return NULL;
        pt->pt2[idx1] = calloc(PT_ENTRIES, sizeof(pte_t));
        if (!pt->pt2[idx1]) {
            fprintf(stderr, "page_table_get_pte: sin memoria\n");
            exit(1);
        }
    }
    return &pt->pt2[idx1][idx2];
}
