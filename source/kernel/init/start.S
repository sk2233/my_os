#include "comm/comm.h"
 	.text
 	.extern kernel_init
 	.global _start
 	.extern main_init
_start:
    call kernel_init
    // 重新加载GDT了进行远跳转
    jmp $KERNEL_SELECTOR_CS,$gdt_reload
gdt_reload:
    mov $KERNEL_SELECTOR_DS,%ax
    mov %ax,%ds
    mov %ax,%ss
    mov %ax,%es
    mov %ax,%fs
    mov %ax,%gs
    call main_init
    jmp .

    .text // 默认处理器不处理有错误码的
    .global exception_default_handler
    .extern handle_default_exception
exception_default_handler:
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    push %esp  // 写入参数指针
    call handle_default_exception
    pop %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    iret

.macro exception_handler name num with_code
    .text
    .extern do_handle_\name
    .global exception_handler_\name
exception_handler_\name:
    .if \with_code == 0
        push $0
    .endif
    push $\num

    pusha
    push %ds
    push %es
    push %fs
    push %gs

    push %esp  // 写入参数指针
    call do_handle_\name
    pop %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    add $(2*4),%esp  // 移除压入的错误码与中断号
    iret
.endm

// 除 0 异常
exception_handler division 0 0
exception_handler time 0x20 0