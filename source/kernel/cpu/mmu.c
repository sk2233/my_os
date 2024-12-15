#include "cpu/mmu.h"
#include "core/mem.h"
#include "tool/mem.h"
#include "comm/cpu_instr.h"

static pde_t kernel_pde[PDE_SIZE] __attribute__((aligned(MEM_PAGE_SIZE))); // 内核页目录表  需要 4k 对齐

uint32_t default_cr3(){
    return (uint32_t)kernel_pde;
}

uint32_t pde_index(uint32_t vaddr) {
    return vaddr >> 22;// 只取高10位 一级页表索引
}

uint32_t pde_paddr (pde_t * pde) {
    return pde->phy_addr << 12; // 获取一级页表对应的地址 高 20 位  低 12 位不需要其是  4k 对齐的
}

uint32_t pte_index(uint32_t vaddr) {
    return (vaddr >> 12) & 0x3FF;   // 取中间10位 二级页表索引
}

uint32_t pte_paddr (pte_t * pte) {
    return pte->phy_addr << 12; // 获取二级页表对应的地址 高 20 位  低 12 位不需要其是  4k 对齐的
}

pte_t *find_pte(pde_t *pde,uint32_t vaddr){
    pde_t *temp = pde+ pde_index(vaddr);
    pte_t *pte;
    if(temp->present){
        pte=(pte_t *) pde_paddr(temp);
    } else{ // 没有动态分配页表
        pte= mem_alloc_page(1); // 必须 4k 对齐
        // 一级权限全部放开，使用二级设置的权限
        temp->v=(uint32_t)pte|PDE_P|PDE_W|PDE_U;
        mem_set(pte,0,MEM_PAGE_SIZE); // 全部初始化为 0
    }
    return pte+ pte_index(vaddr);
}

// 在页表 pde 中创建  vaddr 映射到 paddr 长度为 count 的连续映射  属性为 attr
void mmu_map(pde_t *pde,uint32_t vaddr,uint32_t paddr,int count,uint32_t perm){
    // 连续映射 count 个
    for (int i = 0; i < count; ++i) {
        // 找到要写入的二级页表写入映射数据
        pte_t *pte= find_pte(pde,vaddr);
        pte->v=paddr|perm;
        vaddr+=MEM_PAGE_SIZE; // 继续向后进行
        paddr+=MEM_PAGE_SIZE;
    }
}

void mmu_init(){
    // 初始化内核映射 现有的内核代码在 64k 位置 先进行一一映射
    // 在后续申请内存时需要注意更新页表 这里为了简单 直接映射 0～128m 的内存
    mmu_map(kernel_pde,0,0,256*128,PTE_W|PTE_P);
    write_cr3((uint32_t)kernel_pde); // 设置页表
    // 打开内存分页
    uint32_t cr0 = read_cr0();
    write_cr0(cr0|(1 << 31));
}