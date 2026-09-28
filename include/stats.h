#ifndef STATS_H
#define STATS_H

#include <time.h>

// contrato del equipo, implementacion en src/stats.c (Persona C)
typedef struct {
    long accesses;
    long faults;
    long replacements;
    double fault_ms;
    double total_ms;
} stats_t;

void stats_init(stats_t *s);
double stats_elapsed_ms(clock_t start);
void stats_print(const stats_t *s, const char *policy_name);

#endif
