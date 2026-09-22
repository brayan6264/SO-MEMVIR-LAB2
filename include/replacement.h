#ifndef REPLACEMENT_H
#define REPLACEMENT_H

// contrato del equipo, implementacion en src/replacement_fifo.c y src/replacement_lru.c (Persona B)
typedef struct {
    const char *name;
    void (*on_load)(void *self, int frame);
    void (*on_access)(void *self, int frame);
    int (*select_victim)(void *self);
    void (*destroy)(void *self);
    void *self;
} replacement_policy_t;

replacement_policy_t *replacement_fifo_create(int num_frames);
replacement_policy_t *replacement_lru_create(int num_frames);

#endif
