#ifndef IRQ_H
#define IRQ_H

#include "comm/types.h"
#include "comm/comm.h"
#include "comm/cpu_instr.h"

#define IRQ_PIC_START   0x20

#define PIC0_ICW1		0x0020
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC0_OCW2	    0x0020
#define PIC0_IMR		0x0021

#define PIC1_ICW1		0x00A0
#define PIC1_ICW2		0x00A1
#define PIC1_ICW3		0x00A1
#define PIC1_ICW4		0x00A1
#define PIC1_OCW2		0x00A0
#define PIC1_IMR		0x00A1

#define IRQ0_TIMER          0x20

// 中断门
#define GATE_INT (0xE<<8)
// 是否有效
#define GATE_P (1<<15)
// 等级
#define GATE_DPL0 (0<<13)
#define GATE_DPL3 (3<<13)

#pragma pack(1)
typedef struct gate_desc{
    uint16_t offset0;
    uint16_t selector;
    uint16_t attr;
    uint16_t offset1;
}gate_desc_t;

typedef struct exception_frame{
    uint32_t gs,fs,ex,ds;
    uint32_t edi,esi,ebp,esp,ebx,edx,ecx,eax;
    uint32_t num,err_code;
    uint32_t eip,cs,eflags;
}exception_frame_t;
#pragma pack()

typedef void (irq_handler_t)(void);

void init_idt();
void irq_install(int irq_num,irq_handler_t handler);

void irq_enable(int irq_num);
void irq_disable(int irq_num);
void irq_send_eoi(int irq_num);

uint32_t irq_enter_protection();
void irq_leave_protection(uint32_t state);

#endif
