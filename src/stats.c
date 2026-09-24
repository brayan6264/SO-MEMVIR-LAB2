#include <stdio.h>
#include "stats.h"

void stats_init(stats_t *s) {
    s->accesses = 0;
    s->faults = 0;
    s->replacements = 0;
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
}