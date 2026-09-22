#ifndef STATS_H
#define STATS_H

// contrato del equipo, implementacion en src/stats.c (Persona C)
typedef struct {
    long accesses;
    long faults;
    long replacements;
} stats_t;

void stats_init(stats_t *s);
void stats_print(const stats_t *s, const char *policy_name);

#endif
