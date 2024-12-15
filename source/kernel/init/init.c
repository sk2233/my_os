#include "init.h"
#include "cpu/cpu.h"
#include "dev/time.h"
#include "core/task.h"
#include "ipc/mutex.h"
#include "core/mem.h"
#include "cpu/mmu.h"

void kernel_init(){
    cpu_init();
    time_init();
    mem_init(); // 初始化内存管理
    mmu_init(); // 初始化 内存分页设置
}

static task_t task_test;
static uint8_t task_test_stack[1024];
static task_t task_main;

void task_entry(){
    int count =0;
    for (;;) {
        count++;
    }
}

void main_init(){
    task_manager_init(); // 需要注意关中断，后面用锁实现
    task_init(&task_main,0,"main",0); // 当然任务不用初始化，当他被替换下来后会自动写入数据
    task_init(&task_test,(uint32_t)task_entry,"test",(uint32_t)&task_test_stack[1024]);
    curr_task_init(&task_main);
    sti();
    
    // void *data= mem_alloc_page(1);
    // mem_free(data);
    // data= mem_alloc_page(1);
    // mem_free(data);

    int count =0;
    for (;;) {
        count++;
        task_sleep(1000);
    }
}