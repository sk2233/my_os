#ifndef MEM_H
#define MEM_H

#include "comm/types.h"

void mem_set(void *dest, uint8_t val, int size);
void mem_cpy(void *dest, void *src, int size);

#endif
