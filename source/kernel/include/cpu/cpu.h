#ifndef CPU_H
#define CPU_H

#include "comm/types.h"
#include "comm/comm.h"
#include "comm/cpu_instr.h"

// limit 只有 20 但是要表示 32位 该标记用于标识数据单位
#define SEG_G (1<<15)
// 默认为1 要设置上  32位还是16位 固定 32位
#define SEG_D (1<<14)
// gdt 表项是否有效 可以用于读时复制
#define SEG_P (1<<7)
// 特权级
#define SEG_DPL0 (0<<5)
#define SEG_DPL3 (3<<5)
// 是否系统段
#define SEG_SYS (0<<4)
#define SEG_NORMAL (1<<4)
// 类型 代码段，数据段 读写权限
#define SEG_CODE (1<<3)
#define SEG_DATA (0<<3)
#define SEG_RW (1<<1)

#pragma pack(1)
typedef struct seg_desc{
    uint16_t limit0; // 还有部分 limit 在 attr 里面  总大小 20位
    uint16_t base0;
    uint8_t base1;
    uint16_t attr;
    uint8_t base2;
}seg_desc_t;
#pragma pack()

void cpu_init();

#endif
