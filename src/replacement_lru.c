#include <stdio.h>
#include <stdlib.h>
#include "replacement.h"

typedef struct {
    long *last_used; // last_used[frame] = marca de tiempo logica del ultimo uso
    long counter;    // reloj logico: se incrementa en cada on_load/on_access
    int capacity;
} lru_state_t;

static void lru_on_load(void *self, int frame) {
    lru_state_t *s = self;
    s->last_used[frame] = ++s->counter;
}

static void lru_on_access(void *self, int frame) {
    lru_state_t *s = self;
    s->last_used[frame] = ++s->counter; // hit: se vuelve el mas reciente
}

// select_victim solo se llama cuando phys_mem no tiene marcos libres, asi que
// los "capacity" marcos ya estan todos ocupados y todos tienen last_used valido
static int lru_select_victim(void *self) {
    lru_state_t *s = self;
    int victim = 0;
    for (int i = 1; i < s->capacity; i++) {
        if (s->last_used[i] < s->last_used[victim]) {
            victim = i;
        }
    }
    return victim;
}

static void lru_destroy(void *self) {
    lru_state_t *s = self;
    free(s->last_used);
    free(s);
}

// quien llama a esto es dueno del puntero devuelto: al terminar, llamar
// policy->destroy(policy->self) y despues free(policy)
replacement_policy_t *replacement_lru_create(int num_frames) {
    lru_state_t *state = calloc(1, sizeof(lru_state_t));
    replacement_policy_t *policy = calloc(1, sizeof(replacement_policy_t));
    if (!state || !policy) {
        fprintf(stderr, "replacement_lru_create: sin memoria\n");
        exit(1);
    }

    state->last_used = calloc((size_t)num_frames, sizeof(long));
    if (!state->last_used) {
        fprintf(stderr, "replacement_lru_create: sin memoria\n");
        exit(1);
    }
    state->capacity = num_frames;
    state->counter = 0;

    policy->name = "LRU";
    policy->on_load = lru_on_load;
    policy->on_access = lru_on_access;
    policy->select_victim = lru_select_victim;
    policy->destroy = lru_destroy;
    policy->self = state;
    return policy;
}