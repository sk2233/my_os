#ifndef MMU_H
#define MMU_H

#include "comm/types.h"

#define PDE_SIZE 1024
#define PTE_SIZE 1024

#define PTE_P       (1 << 0)
#define PTE_W           (1 << 1)
#define PTE_U           (1 << 2)

#define PDE_P       (1 << 0)
#define PDE_W           (1 << 1)
#define PDE_U           (1 << 2)

#pragma pack(1)
typedef union pde {
    uint32_t v;
    struct {
        uint32_t present : 1;                   // 0 (P) Present; must be 1 to map a 4-KByte page
        uint32_t write : 1;             // 1 (R/W) Read/write, if 0, writes may not be allowe
        uint32_t user_mode : 1;             // 2 (U/S) if 0, user-mode accesses are not allowed t
        uint32_t write_through : 1;             // 3 (PWT) Page-level write-through
        uint32_t cache_disable : 1;             // 4 (PCD) Page-level cache disable
        uint32_t accessed : 1;                  // 5 (A) Accessed
        uint32_t : 1;                           // 6 Ignored;
        uint32_t ps : 1;                        // 7 (PS)
        uint32_t : 4;                           // 11:8 Ignored
        uint32_t phy_addr : 20;              // 高20位page table物理地址
    };
}pde_t;

typedef union pte {
    uint32_t v;
    struct {
        uint32_t present : 1;                   // 0 (P) Present; must be 1 to map a 4-KByte page
        uint32_t write : 1;             // 1 (R/W) Read/write, if 0, writes may not be allowe
        uint32_t user_mode : 1;             // 2 (U/S) if 0, user-mode accesses are not allowed t
        uint32_t write_through : 1;             // 3 (PWT) Page-level write-through
        uint32_t cache_disable : 1;             // 4 (PCD) Page-level cache disable
        uint32_t accessed : 1;                  // 5 (A) Accessed;
        uint32_t dirty : 1;                     // 6 (D) Dirty
        uint32_t pat : 1;                       // 7 PAT
        uint32_t global : 1;                    // 8 (G) Global
        uint32_t : 3;                           // Ignored
        uint32_t phy_addr : 20;            // 高20位物理地址
    };
}pte_t;
#pragma pack()

void mmu_init();

uint32_t default_cr3();

#endif
