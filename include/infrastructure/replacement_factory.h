#ifndef REPLACEMENT_FACTORY_H
#define REPLACEMENT_FACTORY_H

#include "domain/replacement.h"

// devuelve NULL si name no es "fifo" ni "lru"
replacement_policy_t *replacement_create(const char *name, int num_frames);
void replacement_destroy(replacement_policy_t *policy);

#endif
