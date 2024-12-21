#include "loader.h"

/**
* 使用LBA48位模式读取磁盘
*/
static void read_disk(uint32_t sector, uint16_t sector_count, uint8_t * buf) {
    outb(0x1F6, (uint8_t) (0xE0)); // 主磁盘
    // 读取扇区数量
    outb(0x1F2, (uint8_t) (sector_count >> 8));
    // 扇区号  48bit 我们只用  32bit
    outb(0x1F3, (uint8_t) (sector >> 24));		// LBA参数的24~31位
    outb(0x1F4, (uint8_t) (0));					// LBA参数的32~39位
    outb(0x1F5, (uint8_t) (0));					// LBA参数的40~47位
    // 读取扇区数量
    outb(0x1F2, (uint8_t) (sector_count));
    // 扇区号  48bit 我们只用  32bit
    outb(0x1F3, (uint8_t) (sector));			// LBA参数的0~7位
    outb(0x1F4, (uint8_t) (sector >> 8));		// LBA参数的8~15位
    outb(0x1F5, (uint8_t) (sector >> 16));		// LBA参数的16~23位
    // 检查数据是否准备就绪
    outb(0x1F7, (uint8_t) 0x24);
    // 读取数据
    uint16_t *data_buf = (uint16_t*) buf;
    while (sector_count-- > 0) {
        // 每次扇区读之前都要检查，等待数据就绪
        while ((inb(0x1F7) & 0x88) != 0x8) {}
        // 读取并将数据写入到缓存中
        for (int i = 0; i < SECTOR_SIZE / 2; i++) {
            *data_buf++ = inw(0x1F0);
        }
    }
}

// 解析 elf 文件获取真正的代码地址 实际在 lds 中有标明，但是最好解耦
uint32_t parse_elf_file(uint8_t *elf_file){
    elf_ehdr_t *ehdr=(elf_ehdr_t *)elf_file;
    if(ehdr->ident[0] != 0x7F || ehdr->ident[1] != 'E' || ehdr->ident[2] != 'L' || ehdr->ident[3] != 'F'){
        // 非法格式
        return 0;
    }

    // 依次处理每一项 一个 elf文件总头下有多个项目头
    for (int i = 0; i < ehdr->phnum; ++i) {
        elf_phdr_t *hdr=(elf_phdr_t *)(elf_file+ehdr->phoff)+i;
        if(hdr->type!=PT_LOAD){ // 改类型无需加载进内存
            continue;
        }

        uint8_t *src=elf_file+hdr->offset;
        uint8_t *dest=(uint8_t *)hdr->paddr;
        for (int j = 0; j < hdr->filesz; ++j) { // 拷贝正常数据
            *dest++=*src++;
        }
        for (int j = 0; j < hdr->memsz - hdr->filesz; ++j) { // 拷贝 bss数据 bss 数据仅占位不存储都是 0
            *dest++=0;
        }
    }
    return ehdr->entry; // 返回入口地址
}

void load_kernel(){
    // 认为 kernel 存在在  100 ~ 612 区域，加载到 1m 的内存区域  只是暂时存放 最终还是放在  64k 的位置执行
    read_disk(100,256,(uint8_t *)SYS_KERNEL_LOAD_ADDR); // 加载 elf文件到 1m处
    uint32_t addr = parse_elf_file((uint8_t *)SYS_KERNEL_LOAD_ADDR);
    ((void (*)(void ))addr)();
}