#include "ipc/sem.h"
#include "core/mem.h"
#include "core/task.h"
#include "cpu/irq.h"

void sem_init(sem_t *sem,int count){
    mem_set(sem,0, sizeof(sem_t));
    sem->count=count;
}

void sem_wait(sem_t *sem){ // 获取信号量
    uint32_t state = irq_enter_protection(); // 这里只能通过开关中断保护了
    if(sem->count>0){
        sem->count--;
    } else{ // 无了进行等待
        task_t *task= task_poll(); // 从就绪队列移动到等待队列
        node_t *node= mem_alloc(sizeof(node_t));
        node->data=task;
        list_add(&sem->wait,node);
        task_dispatch(); // 切换执行
    }
    irq_leave_protection(state);
}

void sem_notify(sem_t *sem){ // 释放信号量
    uint32_t state = irq_enter_protection();
    if(list_empty(&sem->wait)){
        sem->count++;
    } else{ // 释放一个唤醒一个
        node_t *node= list_poll(&sem->wait);
        task_add(node->data);
        mem_free(node);
    }
    irq_leave_protection(state);
}
