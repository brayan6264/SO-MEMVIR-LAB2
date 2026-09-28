#include <stdio.h>
#include <stdlib.h>
#include "domain/replacement.h"

typedef struct {
    int *queue;
    int head, tail, count, capacity;
} fifo_state_t;

static void fifo_on_load(void *self, int frame) {
    fifo_state_t *s = self;
    s->queue[s->tail] = frame;
    s->tail = (s->tail + 1) % s->capacity;
    s->count++;
}

static void fifo_on_access(void *self, int frame) {
    (void)self;
    (void)frame; // FIFO no cambia nada cuando ya esta en memoria, solo importa el orden de carga
}

static int fifo_select_victim(void *self) {
    fifo_state_t *s = self;
    int victim = s->queue[s->head];
    s->head = (s->head + 1) % s->capacity;
    s->count--;
    return victim;
}

static void fifo_remove_at(fifo_state_t *s, int index) {
    for (int i = index; i < s->count - 1; i++) {
        int current = (s->head + i) % s->capacity;
        int next = (s->head + i + 1) % s->capacity;
        s->queue[current] = s->queue[next];
    }
    s->tail = (s->tail - 1 + s->capacity) % s->capacity;
    s->count--;
}

static void fifo_on_release(void *self, int frame) {
    fifo_state_t *s = self;
    for (int i = 0; i < s->count; i++) {
        if (s->queue[(s->head + i) % s->capacity] == frame) {
            fifo_remove_at(s, i);
            return;
        }
    }
}

static void fifo_destroy(void *self) {
    fifo_state_t *s = self;
    free(s->queue);
    free(s);
}

// quien llama a esto es dueno del puntero devuelto: al terminar, llamar
// policy->destroy(policy->self) y despues free(policy)
replacement_policy_t *replacement_fifo_create(int num_frames) {
    fifo_state_t *state = calloc(1, sizeof(fifo_state_t));
    replacement_policy_t *policy = calloc(1, sizeof(replacement_policy_t));
    if (!state || !policy) {
        fprintf(stderr, "replacement_fifo_create: sin memoria\n");
        exit(1);
    }

    state->queue = malloc(sizeof(int) * (size_t)num_frames);
    state->capacity = num_frames;

    policy->name = "FIFO";
    policy->on_load = fifo_on_load;
    policy->on_access = fifo_on_access;
    policy->select_victim = fifo_select_victim;
    policy->on_release = fifo_on_release;
    policy->destroy = fifo_destroy;
    policy->self = state;
    return policy;
}
