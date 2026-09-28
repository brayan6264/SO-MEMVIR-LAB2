#ifndef PAGE_CONFIG_H
#define PAGE_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t page_size;
    uint32_t offset_bits;
    uint32_t pt1_entries;
    uint32_t pt2_entries;
} page_config_t;

// false si page_size no es potencia de 2 dentro de [MIN_PAGE_SIZE, MAX_PAGE_SIZE]
bool page_config_init(page_config_t *cfg, uint32_t page_size);
uint32_t page_config_pt1_index(const page_config_t *cfg, uint32_t vaddr);
uint32_t page_config_pt2_index(const page_config_t *cfg, uint32_t vaddr);
uint32_t page_config_offset(const page_config_t *cfg, uint32_t vaddr);

#endif
