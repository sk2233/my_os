#include "core/task.h"
#include "comm/comm.h"
#include "cpu/cpu.h"
#include "dev/time.h"

static task_manager_t task_manager;
static task_t task_idle;
static uint8_t task_idle_stack[1024];

void idle_task(){
    for (;;) {
        hlt();
    }
}

void task_manager_init(){
    mem_set(&task_manager,0, sizeof(task_manager_t));
}

void curr_task_init(task_t *task){
    // 初始化 idle 进程  为了保证顺序只能在这里搞了
    task_init(&task_idle,(uint32_t)idle_task,"idle",(uint32_t)&task_idle_stack[1024]);

    task_manager.curr_task=task;
    write_tr(task->tss_sel);
}

void task_dispatch(){
    node_t *node = list_peek(&task_manager.ready);
    if(node->data==task_manager.curr_task){ // 防止来回触发
        task_manager.curr_task->last_tick=task_manager.curr_task->tick;
        return;
    }

    task_manager.curr_task=node->data;
    task_manager.curr_task->last_tick=task_manager.curr_task->tick;
    switch_task(task_manager.curr_task->tss_sel);
}

void task_yield(){
    if(task_manager.ready.count<2){ // 防止来回触发
        task_manager.curr_task->last_tick=task_manager.curr_task->tick;
        return;
    }

    node_t *node= list_poll(&task_manager.ready);
    list_add(&task_manager.ready,node);
    task_dispatch();
}

task_t *curr_task(){
    return task_manager.curr_task;
}

void task_init(task_t *task,uint32_t entry,const char *name,uint32_t esp){
    task->tss_sel=gdt_alloc_desc();
    seg_desc_set(task->tss_sel, (uint32_t)&task->tss, sizeof(tss_t),
                     SEG_P | SEG_DPL0 | SEG_TSS);

    mem_cpy(task->name,(void *)name,TASK_NAME_SIZE);
    task->tick=10; // 总时间片
    task->last_tick=task->tick;

    mem_set(&task->tss,0, sizeof(tss_t));
    task->tss.eip=entry;
    task->tss.esp=task->tss.esp0=esp;
    task->tss.ss=task->tss.ss0=task->tss.es=task->tss.ds=task->tss.fs=task->tss.gs=KERNEL_SELECTOR_DS;
    task->tss.cs=KERNEL_SELECTOR_CS;
    task->tss.eflags=EFLAGS_DEFAULT|EFLAGS_IF;

    task_add(task);
}

void switch_task(int tss_sel){
    far_jump(tss_sel, 0);
}

void task_tick(){
    node_t *node= list_peek(&task_manager.ready);
    task_t *task=node->data;
    task->last_tick--;
    if(task->last_tick<0){
        task_yield();
    }

    while (!list_empty(&task_manager.wait)){
        node= list_peek(&task_manager.wait);
        if(node->order>sys_tick()){
            break;
        } // 只是添加并不立即执行
        node= list_poll(&task_manager.wait);
        list_add(&task_manager.ready,node);
    }
}

void task_sleep(int ms){
    int tick=(ms+OS_TICK_MS-1)/OS_TICK_MS;
    if(tick<1){
        return;
    }

    node_t *node= list_poll(&task_manager.ready);
    task_t *task=node->data;
    task->wake_tick=sys_tick()+tick;
    node->order=task->wake_tick;
    list_order_add(&task_manager.wait,node);

    task_dispatch();
}

task_t *task_poll(){
    node_t *node= list_poll(&task_manager.ready);
    task_t *res=node->data;
    mem_free(node);
    return res;
}

void task_add(task_t *task){
    node_t *node= mem_alloc(sizeof(node_t));
    node->data=task;
    list_add(&task_manager.ready,node);
}