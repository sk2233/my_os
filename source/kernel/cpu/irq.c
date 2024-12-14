#include "cpu/irq.h"

static gate_desc_t idt_table[IDT_TABLE_SIZE];

void gate_desc_set(int index,uint16_t selector,uint32_t offset,uint16_t attr){
    gate_desc_t *desc = idt_table+index;

    desc->offset0=offset&0xFFFF;
    desc->selector=selector;
    desc->attr=attr;
    desc->offset1=(offset>>16)&0xFFFF;
}

void irq_install(int irq_num,irq_handler_t handler){
    gate_desc_set(irq_num,KERNEL_SELECTOR_CS,(uint32_t)handler,
                  GATE_P|GATE_DPL0|GATE_INT);
}

void exception_handler_division();

void init_irq(){
    irq_install(0,exception_handler_division);
}

void do_handle_division(exception_frame_t *frame){
    // 除 0 异常
}

void init_pic(){
    // 设置第一个芯片
    outb(PIC0_ICW1,0x11); /* 边缘触发模式（edge trigger mode） */
    outb(PIC0_ICW2,IRQ_PIC_START); // 中断号分配
    outb(PIC0_ICW3,1 <2); // 主
    outb(PIC0_ICW4,0x01);/* 无缓冲区模式 */
    // 设置第 2 个芯片
    outb(PIC1_ICW1,0x11); /* 边缘触发模式（edge trigger mode） */
    outb(PIC1_ICW2,IRQ_PIC_START+8); // 中断号分配
    outb(PIC1_ICW3,2); // 从
    outb(PIC1_ICW4,0x01);
    // 屏蔽中断信号
    outb(PIC0_IMR,0xFF&~(1 <2));
    outb(PIC1_IMR,0xFF);
}

void exception_default_handler();

void init_idt(){
    for (int i = 0; i < IDT_TABLE_SIZE; ++i) {
        gate_desc_set(i,KERNEL_SELECTOR_CS,(uint32_t)exception_default_handler,
                      GATE_P|GATE_DPL0|GATE_INT);
    }
    init_irq();
    init_pic();
    lidt((uint32_t)idt_table, sizeof(idt_table));
}

void handle_default_exception(exception_frame_t *frame){
    for (;;) { // 默认异常处理函数
        hlt();
    }
}

void irq_enable(int irq_num){
    irq_num-=IRQ_PIC_START;
    if(irq_num<8){
        uint8_t mask= inb(PIC0_IMR)&~(1<<irq_num); // 设置为 0 就是 开启
        outb(PIC0_IMR,mask);
    } else{
        irq_num-=8;
        uint8_t mask= inb(PIC1_IMR)&~(1<<irq_num);
        outb(PIC1_IMR,mask);
    }
}

void irq_disable(int irq_num){
    irq_num-=IRQ_PIC_START;
    if(irq_num<8){
        uint8_t mask= inb(PIC0_IMR)|(1<<irq_num);
        outb(PIC0_IMR,mask);
    } else{
        irq_num-=8;
        uint8_t mask= inb(PIC1_IMR)|(1<<irq_num);
        outb(PIC1_IMR,mask);
    }
}

void irq_send_eoi(int irq_num){
    irq_num -= IRQ_PIC_START;
    if (irq_num >= 8) { // 从片也可能需要发送EOI
        outb(PIC1_OCW2, 1 << 5);
    }
    outb(PIC0_OCW2, 1 << 5);
}

uint32_t irq_enter_protection(){
    uint32_t state = read_eflags();
    cli(); // 实际就是改  eflags
    return state;
}

void irq_leave_protection(uint32_t state){ // 不能直接通过开关中断需要通过记录状态完成
    write_eflags(state);
}