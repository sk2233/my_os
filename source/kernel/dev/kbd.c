#include "dev/kbd.h"
#include "cpu/irq.h"
#include "dev/console.h"

void kbd_init(){
    irq_enable(IRQ0_KBD);
    irq_install(IRQ0_KBD, exception_handler_kbd);
}

boot_t get_press(uint8_t data){ // 通过第 7 位判断是否按下
    return !(data&0x80);
}

uint8_t get_key(uint8_t data){ // 剩下 7 位为 code 值
    return data&0x7F;
}

void do_handle_kbd(exception_frame_t *frame) {
    irq_send_eoi(IRQ0_KBD); // 必须响应才声明能处理下一个了
    // 检查是否有数据，无数据则退出
    uint8_t status = inb(KBD_PORT_STAT);
    if (!(status & KBD_STAT_READY)) {
        return;
    }
    uint8_t data = inb(KBD_PORT_DATA);
    boot_t press= get_press(data);
    uint8_t key = get_key(data);
    if(press){
        console_write("KEY press\n",20);
    } else{
        console_write("KEY release\n",20);
    }
}

