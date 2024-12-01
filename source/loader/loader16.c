__asm__(".code16gcc");

#include "loader.h"

static uint16_t gdt_table[][4]={
        {0,0,0,0},
        {0xFFFF,0x0000,0x9A00,0x00CF},
        {0xFFFF,0x0000,0x9200,0x00CF},
};

void protect_mode_entry();

void show_msg(const char *msg) {
    char ch;
    while ((ch = *msg++)!= '\0') {
        __asm__ __volatile__(
                "mov $0xe,%%ah\n\t"
                "mov %[ch],%%al\n\t"
                "int $0x10"::[ch]"r"(ch)
                );
    }
}

void enter_protect_mode(){
    cli();
    // 打开 A20 地址线
    uint8_t res = inb(0x92);
    outb(0x92,res|0x2);
    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
    uint32_t cr0 = read_cr0();
    write_cr0(cr0|(1<<0));
    far_jump(8,(uint32_t)protect_mode_entry);
}

void loader_entry() {
    show_msg("loading...");
    enter_protect_mode();
}