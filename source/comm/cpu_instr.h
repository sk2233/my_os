#ifndef CPU_INSTR_H
#define CPU_INSTR_H

#include "types.h"

static inline void cli(){
    __asm__ __volatile__("cli");
}

static inline void sti(){
    __asm__ __volatile__("sti");
}

static inline uint8_t inb(uint16_t port){
    uint8_t res;
    __asm__ __volatile__("inb %[p],%[v]":[v]"=a"(res):[p]"d"(port));
    return res;
}

static inline void outb(uint16_t port,uint8_t data){
    __asm__ __volatile__("outb %[v],%[p]"::[p]"d"(port),[v]"a"(data));
}

static inline uint16_t inw(uint16_t  port) {
    uint16_t res;
    __asm__ __volatile__("in %[p],%[v]":[v]"=a"(res):[p]"d"(port));
    return res;
}

static inline void lgdt(uint32_t start,uint16_t size){
    struct {
        uint16_t limit;
        uint16_t start1;
        uint16_t start2;
    }gdt;  // gdt 48位
    gdt.start1=start&0xFFFF;
    gdt.start2=start>>16;
    gdt.limit=size-1;

    __asm__ __volatile__("lgdt %[g]"::[g]"m"(gdt));
}

static inline void lidt(uint32_t start, uint32_t size) {
    struct {
        uint16_t limit;
        uint16_t start1;
        uint16_t start2;
    } idt;
    idt.start2 = start >> 16;
    idt.start1 = start & 0xFFFF;
    idt.limit = size - 1;

    __asm__ __volatile__("lidt %[i]"::[i]"m"(idt));
}

static inline uint32_t read_cr0() {
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %[v]":[v]"=r"(cr0));
    return cr0;
}

static inline void write_cr0(uint32_t cr0) {
    __asm__ __volatile__("mov %[v], %%cr0"::[v]"r"(cr0));
}

static inline void far_jump(uint32_t selector, uint32_t offset) {
    uint32_t addr[] = {offset, selector };
    __asm__ __volatile__("ljmpl *(%[a])"::[a]"r"(addr));
}

static inline void hlt() {
    __asm__ __volatile__("hlt");
}

static inline void write_tr(uint32_t tss_sel) {
    __asm__ __volatile__("ltr %[i]"::[i]"m"(tss_sel));
}

static inline uint32_t read_eflags() {
    uint32_t eflags;
    __asm__ __volatile__("pushfl\n\tpopl %%eax":"=a"(eflags));
    return eflags;
}

static inline void write_eflags(uint32_t eflags) {
    __asm__ __volatile__("pushl %%eax\n\tpopfl"::"a"(eflags));
}

#endif
