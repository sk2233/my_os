#include "core/mem.h"
#include "tool/bitmap.h"
#include "tool/mem.h"

// 从 1m 开始到 126m 用于自由分配  当前系统代码已经转移到  64k 的位置
static uint8_t mask[MEM_SIZE/MEM_PAGE_SIZE/8];
static bitmap_t bitmap;

static mem_t mems[MEM_COUNT];

void add_mem(void *res,int index,int count){
    for (int i = 0; i < MEM_COUNT; ++i) {
        mem_t *mem=mems+i;
        if(mem->count==0){
            mem->res=res;
            mem->index=index;
            mem->count=count;
            break;
        }
    }
    // 出问题了
}

mem_t *find_mem(void *res){
    for (int i = 0; i < MEM_COUNT; ++i) {
        mem_t *mem=mems+i;
        if(mem->res==res){
            return mem;
        }
    }
    return NULL;
}

void mem_init(){
    mem_set(mask,0, sizeof(mask));
    bitmap_init(&bitmap,mask,MEM_SIZE/MEM_PAGE_SIZE);
    mem_set(mems,0, sizeof(mems));
}

// 对于进程最好自己维护自己的一套虚拟地址空间，对于内存申请 注册其虚拟地址与物理地址的映射关系到页表
void *mem_alloc_page(int page){ // 按页分配
    int index = bitmap_find_empty(&bitmap,page);
    // 标记被占用了
    for (int i = 0; i < page; ++i) {
        bitmap_set(&bitmap,index+i,TRUE);
    }
    void *res = (void *)(MEM_ADDR+index*MEM_PAGE_SIZE);
    // 初始化返回
    mem_set(res,0,page*MEM_PAGE_SIZE);
    add_mem(res,index,page); // 记录一下
    return res;
}

void *mem_alloc(int size){
    return mem_alloc_page((size+MEM_PAGE_SIZE-1)/MEM_PAGE_SIZE);
}

void mem_free(void *res){
    mem_t *mem= find_mem(res); // 查表清除
    for (int i = 0; i < mem->count; ++i) {
        bitmap_set(&bitmap,mem->index+i,FALSE);
    }
    mem_set(mem,0, sizeof(mem_t));
}