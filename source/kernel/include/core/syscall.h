#ifndef SYSCALL_H
#define SYSCALL_H

#include "comm/types.h"

#define SYSCALL_PARAM_COUNT     5       	// 系统调用最大支持的参数

typedef struct syscall_args {
    int id;
    int arg0;
    int arg1;
    int arg2;
    int arg3;
}syscall_args_t;

void syscall_handler();		// syscall处理

int sys_call(syscall_args_t * args);

#endif
