#include "page_config.h"
#include "config.h"

static bool is_power_of_two(uint32_t value) {
    return value != 0 && (value & (value - 1)) == 0;
}

static uint32_t log2_of(uint32_t value) {
    uint32_t bits = 0;
    while ((1u << bits) < value) {
        bits++;
    }
    return bits;
}

bool page_config_init(page_config_t *cfg, uint32_t page_size) {
    if (!is_power_of_two(page_size) || page_size < MIN_PAGE_SIZE || page_size > MAX_PAGE_SIZE) {
        return false;
    }
    cfg->page_size = page_size;
    cfg->offset_bits = log2_of(page_size);
    cfg->pt2_entries = 1u << PT2_BITS;
    cfg->pt1_entries = 1u << (VADDR_BITS - PT2_BITS - cfg->offset_bits);
    return true;
}

uint32_t page_config_pt1_index(const page_config_t *cfg, uint32_t vaddr) {
    return vaddr >> (PT2_BITS + cfg->offset_bits);
}

uint32_t page_config_pt2_index(const page_config_t *cfg, uint32_t vaddr) {
    return (vaddr >> cfg->offset_bits) & (cfg->pt2_entries - 1);
}

uint32_t page_config_offset(const page_config_t *cfg, uint32_t vaddr) {
    return vaddr & (cfg->page_size - 1);
}
