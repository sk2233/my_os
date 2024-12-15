#ifndef BITMAP_H
#define BITMAP_H

#include "comm/types.h"

typedef struct bitmap{
    uint8_t *mask; // 按需要申请
    int count;
}bitmap_t;

void bitmap_init(bitmap_t *bitmap,uint8_t *mask,int count);
void bitmap_set(bitmap_t *bitmap,int index,boot_t val);
boot_t bitmap_get(bitmap_t *bitmap,int index);
int bitmap_find_empty(bitmap_t *bitmap,int count);

#endif
