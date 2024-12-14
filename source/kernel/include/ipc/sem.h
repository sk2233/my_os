#ifndef SEM_H
#define SEM_H

#include "comm/types.h"
#include "tool/list.h"

typedef struct sem{
    int count;
    list_t wait;
}sem_t;

void sem_init(sem_t *sem,int count);
void sem_wait(sem_t *sem);
void sem_notify(sem_t *sem);

#endif
