#include "dev/time.h"
#include "cpu/irq.h"
#include "core/task.h"

static int tick;

void exception_handler_time();

void time_init(){
    uint32_t reload_count = PIT_OSC_FREQ *OS_TICK_MS/1000;
    outb(PIT_COMMAND_MODE_PORT, (0<<6) | (3<<4) | (3<<1));
    outb(PIT_CHANNEL0_DATA_PORT, reload_count & 0xFF);   // 加载低8位
    outb(PIT_CHANNEL0_DATA_PORT, (reload_count >> 8) & 0xFF); // 再加载高8位

    irq_enable(IRQ0_TIMER);
    irq_install(IRQ0_TIMER,exception_handler_time);
}

int sys_tick(){
    return tick;
}

void do_handle_time(exception_frame_t *frame){
    tick++;
    irq_send_eoi(IRQ0_TIMER); // 必须响应才声明能处理下一个了
    task_tick();
}

