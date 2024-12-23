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

boot_t mem_eq(void *val1,void *val2,int size){
    uint8_t *v1 = (uint8_t *)val1;
    uint8_t *v2 = (uint8_t *)val2;
    while (size--){
        if(*v1!=*v2){
            return FALSE;
        }
        v1++;
        v2++;
    }
    return TRUE;
}

int str_len(const char *str){
    int len=0;
    while (*str){
        len++;
        str++;
    }
    return len;
}