#include "init.h"
#include "cpu/cpu.h"
#include "dev/time.h"

void kernel_init(){
    cpu_init();
    time_init();
}

void main_init(){
    sti();
    for (;;) {

    }
}