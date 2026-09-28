#include <stdio.h>
#include "domain/stats.h"

void stats_init(stats_t *s) {
    s->accesses = 0;
    s->faults = 0;
    s->replacements = 0;
    s->fault_ms = 0.0;
    s->total_ms = 0.0;
}

double stats_elapsed_ms(clock_t start) {
    return (double)(clock() - start) * 1000.0 / CLOCKS_PER_SEC;
}

void stats_print(const stats_t *s, const char *policy_name) {
    // evita division por cero si nunca se hizo ningun acceso
    double hit_rate = 0.0;
    if (s->accesses > 0) {
        hit_rate = (double)(s->accesses - s->faults) / (double)s->accesses * 100.0;
    }

    printf("Total de accesos: %ld\n", s->accesses);
    printf("Total fallos de pagina: %ld\n", s->faults);
    printf("Hit rate: %.2f%%\n", hit_rate);
    printf("Total reemplazos: %ld\n", s->replacements);
    printf("Politica: %s\n", policy_name);
    printf("Tiempo en fallos de pagina: %.3f ms\n", s->fault_ms);
    printf("Tiempo total: %.3f ms\n", s->total_ms);
}