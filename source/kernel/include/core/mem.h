#ifndef CMEM_H
#define CMEM_H

// 从 1m 开始到 126m 用于自由分配  由虚拟机配置决定

#include "comm/types.h"

#define MEM_PAGE_SIZE 4096
#define MEM_SIZE (1024*1024*126)
#define MEM_ADDR 0x100000

#define MEM_COUNT 1024

typedef struct mem{
    void *res;
    int index;
    int count;
}mem_t;

void mem_init();
void *mem_alloc(int size);
void mem_free(void *mem);
void *mem_alloc_page(int page);
void mem_occupy(void *mem,int size);

#endif
