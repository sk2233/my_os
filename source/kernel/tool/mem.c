#include "tool/mem.h"

void mem_set(void *dest, uint8_t val, int size){
    if (!dest||!size) {
        return;
    }
    uint8_t * temp = (uint8_t *)dest;
    while (size--) {
        *temp++ = val;
    }
}

void mem_cpy(void *dest, void *src, int size) {
    if (!dest || !src || !size) {
        return;
    }

    uint8_t *s = (uint8_t *)src;
    uint8_t *d = (uint8_t *)dest;
    while (size--) {
        *d++ = *s++;
    }
}