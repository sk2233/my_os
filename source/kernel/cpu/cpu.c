#include "cpu/cpu.h"
#include "cpu/irq.h"

static seg_desc_t gdt_table[GDT_TABLE_SIZE];

int gdt_alloc_desc(){
    // 0不能占用
    for (int i = 1; i < GDT_TABLE_SIZE; i++) {
        seg_desc_t *desc = gdt_table + i;
        if (desc->attr == 0) {
            return i<<3;
        }
    }
    return -1;
}

void seg_desc_set(int selector,uint32_t base,uint32_t limit,uint16_t attr){
    seg_desc_t *desc=gdt_table+(selector>>3); // 之所以偏移是因为段选择子下标是从第 3 位开始的，具体参考段选择子结构
    if(limit>0xFFFFF){
        limit/=0x1000;
        attr|=SEG_G;
    }

    desc->limit0= limit & 0xFFFF;
    desc->base0=base&0xFFFF;
    desc->base1=(base>>16)&0xFF;
    desc->attr=attr|(((limit>>16)&0xF)<<8);
    desc->base2=(base>>24)&0xFF;
}

void init_gdt(){
    for (int i = 0; i < GDT_TABLE_SIZE; ++i) {
        seg_desc_set(i<<3,0,0,0);
    }
    // 设置代码段与数据段
    seg_desc_set(KERNEL_SELECTOR_DS,0,0xFFFFFFFF,
                 SEG_P|SEG_DPL0|SEG_NORMAL|SEG_DATA|SEG_RW|SEG_D);
    seg_desc_set(KERNEL_SELECTOR_CS,0,0xFFFFFFFF,
                 SEG_P|SEG_DPL0|SEG_NORMAL|SEG_CODE|SEG_RW|SEG_D);
    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
}

void cpu_init(){
    init_gdt();
    init_idt();
}

