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

static uint32_t addr=0x200000; // 从 2m 开始分配

void *mem_alloc(int size){ // TODO 先临时使用
    void *res=(void *)addr;
    mem_set(res,0,size);
    addr+=size;
    return res;
}

void mem_free(void *mem){ // TODO 先临时使用

}