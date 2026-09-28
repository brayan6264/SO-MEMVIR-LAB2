#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <stdint.h>
#include <stdbool.h>
#include "page_config.h"

typedef struct {
    uint32_t frame;
    bool valid;
    bool accessed;
    bool dirty;
} pte_t;

typedef struct page_table page_table_t;

page_table_t *page_table_create(const page_config_t *cfg);
void page_table_destroy(page_table_t *pt);
const page_config_t *page_table_config(const page_table_t *pt);
// create=true crea la tabla de nivel 2 si no existe; create=false devuelve NULL si no existe
pte_t *page_table_get_pte(page_table_t *pt, uint32_t vaddr, bool create);

#endif
