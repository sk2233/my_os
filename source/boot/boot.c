__asm__(".code16gcc");

#include "boot.h"

/**
 * Boot的C入口函数
 * 只完成一项功能，即从磁盘找到loader文件然后加载到内容中，并跳转过去
 */
void boot_entry() { // 直接根据地址跳转
    ((void (*)(void))LOADER_ADDR)();
} 

