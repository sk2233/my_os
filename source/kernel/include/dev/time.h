#ifndef TIME_H
#define TIME_H

#include "comm/types.h"

#define PIT_OSC_FREQ                1193182				// 定时器时钟

// 定时器的寄存器和各项位配置
#define PIT_CHANNEL0_DATA_PORT       0x40
#define PIT_COMMAND_MODE_PORT        0x43

void time_init();
int sys_tick();

#endif
