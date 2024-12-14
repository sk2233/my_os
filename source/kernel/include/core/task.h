#ifndef TASK_H
#define TASK_H

#include "comm/types.h"
#include "tool/mem.h"
#include "tool/list.h"

#define EFLAGS_IF           (1 << 9)
#define EFLAGS_DEFAULT      (1 << 1)

#define TASK_NAME_SIZE  16

#pragma pack(1)
typedef struct tss {
    uint32_t pre_link;
    uint32_t esp0, ss0, esp1, ss1, esp2, ss2;
    uint32_t cr3;
    uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint32_t iomap;
}tss_t;
#pragma pack()

typedef struct task {
    char name[TASK_NAME_SIZE];
    tss_t tss;				// 任务的TSS段
    int tss_sel;		// tss选择子
    int tick;    // 每次执行的时间
    int last_tick; //  还剩余要执行的时间
    int wake_tick; //  要被唤醒的时间
}task_t;

typedef struct task_manager{
    task_t *curr_task;
    list_t ready;
    list_t wait;
}task_manager_t;

void task_manager_init();
void curr_task_init(task_t *task);
void task_yield();
task_t *curr_task();

void task_init(task_t *task,uint32_t entry,const char *name,uint32_t esp);
void switch_task(int tss_sel);
void task_tick();
void task_sleep(int ms);
void task_dispatch();

task_t *task_poll();
void task_add(task_t *task);


#endif
