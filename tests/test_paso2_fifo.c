#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "translate.h"
#include "config.h"

// prueba de integracion real (page_table + translate + phys_mem + replacement_fifo + page_fault)
// con memoria fisica de solo 2 marcos, para forzar reemplazos

int main(void) {
    page_table_t *pt = page_table_create();
    phys_mem_t *pm = phys_mem_create(2 * PAGE_SIZE);
    replacement_policy_t *policy = replacement_fifo_create(2);
    stats_t stats;
    stats_init(&stats);

    uint32_t pa0 = translate(pt, pm, policy, &stats, 0, true);            // fallo, usa marco 0
    uint32_t pa1 = translate(pt, pm, policy, &stats, PAGE_SIZE, true);    // fallo, usa marco 1
    uint32_t pa2 = translate(pt, pm, policy, &stats, 2 * PAGE_SIZE, true); // fallo, sin marco libre: desaloja el 0
    uint32_t pa0b = translate(pt, pm, policy, &stats, 0, false);          // pagina 0 ya no esta: fallo, desaloja el 1

    assert(pa0 == 0);
    assert(pa1 == PAGE_SIZE);
    assert(pa2 == 0);            // reusa el marco 0
    assert(pa0b == PAGE_SIZE);   // reusa el marco 1

    assert(stats.accesses == 4);
    assert(stats.faults == 4);
    assert(stats.replacements == 2);

    printf("paso 1 + paso 2 (fifo) integrados: todas las pruebas pasaron\n");
    stats_print(&stats, policy->name);

    policy->destroy(policy->self);
    free(policy);
    phys_mem_destroy(pm);
    page_table_destroy(pt);
    return 0;
}
