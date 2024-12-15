#include "ipc/mutex.h"
#include "cpu/irq.h"
#include "core/mem.h"

void mutex_init(mutex_t *mutex){
    mem_set(mutex,0, sizeof(mutex_t));
}

void mutex_lock(mutex_t *mutex){
    uint32_t state = irq_enter_protection(); // 这里只能通过开关中断保护了
    task_t *task=curr_task();
    if(mutex->lock_count==0){
        mutex->owner=task;
        mutex->lock_count++;
    } else if(mutex->owner==task){
        mutex->lock_count++;
    } else{ // 无了
        task= task_poll(); // 从就绪队列移动到等待队列
        node_t *node= mem_alloc(sizeof(node_t));
        node->data=task;
        list_add(&mutex->wait,node);
        task_dispatch(); // 切换执行
    }
    irq_leave_protection(state);
}

void mutex_unlock(mutex_t *mutex){
    uint32_t state = irq_enter_protection(); // 这里只能通过开关中断保护了
    task_t *task=curr_task();
    if(mutex->owner==task&&mutex->lock_count>0){
        mutex->lock_count--;
        if(mutex->lock_count==0&& !list_empty(&mutex->wait)){
            node_t *node= list_poll(&mutex->wait);
            task=node->data;
            mem_free(node);
            task_add(task);
            // 立即给等待的进程
            mutex->owner=task;
            mutex->lock_count++;
        }
    }
    irq_leave_protection(state);
}