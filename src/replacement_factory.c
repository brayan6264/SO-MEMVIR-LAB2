#include <stdlib.h>
#include <string.h>
#include "replacement.h"

replacement_policy_t *replacement_create(const char *name, int num_frames) {
    if (strcmp(name, "fifo") == 0) return replacement_fifo_create(num_frames);
    if (strcmp(name, "lru") == 0) return replacement_lru_create(num_frames);
    return NULL;
}

void replacement_destroy(replacement_policy_t *policy) {
    policy->destroy(policy->self);
    free(policy);
}
