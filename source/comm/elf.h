#ifndef ELF_H
#define ELF_H

#include "types.h"

#define PT_LOAD 1

#pragma pack(1) // 防止自动对齐编译添加内容
typedef struct elf_ehdr{ // elf 文件头
    char ident[16]; // 魔数标识符
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
}elf_ehdr_t;

typedef struct elf_phdr{
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
}elf_phdr_t;

#pragma pack()

#endif
