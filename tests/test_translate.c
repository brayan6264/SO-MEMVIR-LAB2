#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "translate.h"
#include "config.h"

// mocks temporales de los modulos de B y C, solo para probar translate.c en aislamiento

struct phys_mem { int next_frame; };

phys_mem_t *phys_mem_create(size_t phys_mem_bytes) {
    (void)phys_mem_bytes;
    phys_mem_t *pm = malloc(sizeof(phys_mem_t));
    pm->next_frame = 0;
    return pm;
}
void phys_mem_destroy(phys_mem_t *pm) { free(pm); }
bool phys_mem_has_free_frame(phys_mem_t *pm) { return pm->next_frame < 4; }
int phys_mem_alloc_frame(phys_mem_t *pm, pte_t *owner) { (void)owner; return pm->next_frame++; }
void phys_mem_free_frame(phys_mem_t *pm, int frame) { (void)pm; (void)frame; }
pte_t *phys_mem_owner(phys_mem_t *pm, int frame) { (void)pm; (void)frame; return NULL; }

static void mock_on_load(void *self, int frame) { (void)self; (void)frame; }
static void mock_on_access(void *self, int frame) { (void)self; (void)frame; }
static int mock_select_victim(void *self) { (void)self; return 0; }
static void mock_destroy(void *self) { (void)self; }

int page_fault_handle(phys_mem_t *pm, replacement_policy_t *policy, pte_t *new_owner, stats_t *stats) {
    (void)new_owner;
    stats->faults++;
    int frame;
    if (phys_mem_has_free_frame(pm)) {
        frame = phys_mem_alloc_frame(pm, new_owner);
    } else {
        frame = policy->select_victim(policy->self);
        stats->replacements++;
    }
    policy->on_load(policy->self, frame);
    return frame;
}

void stats_init(stats_t *s) { s->accesses = s->faults = s->replacements = 0; }
void stats_print(const stats_t *s, const char *policy_name) {
    printf("accesses=%ld faults=%ld replacements=%ld policy=%s\n",
           s->accesses, s->faults, s->replacements, policy_name);
}

int main(void) {
    replacement_policy_t policy = {
        .name = "MOCK", .on_load = mock_on_load, .on_access = mock_on_access,
        .select_victim = mock_select_victim, .destroy = mock_destroy, .self = NULL
    };
    page_table_t *pt = page_table_create();
    phys_mem_t *pm = phys_mem_create(256 * 1024);
    stats_t stats;
    stats_init(&stats);

    uint32_t pa1 = translate(pt, pm, &policy, &stats, 0, true);    // fallo: primer acceso a la pagina 0
    uint32_t pa2 = translate(pt, pm, &policy, &stats, 4096, true); // fallo: pagina distinta
    uint32_t pa3 = translate(pt, pm, &policy, &stats, 0, false);   // hit: misma pagina que pa1

    assert(pa1 == 0);
    assert(pa2 == PAGE_SIZE);
    assert(pa3 == pa1);
    assert(stats.accesses == 3);
    assert(stats.faults == 2);
    assert(stats.replacements == 0);

    uint32_t vaddr_cross_pt1 = (1u << 22); // pt1=1, pt2=0, offset=0: fuerza otra tabla de nivel 2
    uint32_t pa4 = translate(pt, pm, &policy, &stats, vaddr_cross_pt1, true);
    assert(pa4 == 2 * PAGE_SIZE);

    printf("todas las pruebas de translate pasaron\n");
    stats_print(&stats, "MOCK");

    page_table_destroy(pt);
    phys_mem_destroy(pm);
    return 0;
}
