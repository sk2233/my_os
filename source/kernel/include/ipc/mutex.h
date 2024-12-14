#ifndef MUTEX_H
#define MUTEX_H

#include "comm/types.h"
#include "core/task.h"
#include "tool/list.h"

typedef struct mutex{
    task_t *owner;
    int lock_count;
    list_t wait;
}mutex_t;

void mutex_init(mutex_t *mutex);
void mutex_lock(mutex_t *mutex);
void mutex_unlock(mutex_t *mutex);

#endif
