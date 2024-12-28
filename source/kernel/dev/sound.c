#include "dev/sound.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "core/fs.h"
#include "core/mem.h"

static boot_t sound_ready;

void exception_handler_sound();

void sound_init(){
    // 设置初始化
    outb(SOUND_RESET_PORT,0x01);
    outb(SOUND_RESET_PORT,0x00); // 一般需要等待 3ms 再发送但是因为虚拟机模拟无需等待
    // 等待初始化完成
    while ((inb(SOUND_STATE_PORT)&0x80)==0){}
    uint8_t data = inb(SOUND_READ_PORT); // 0xAA 准备就绪
    sound_ready=data==0xAA;

    // 添加中断 判断缓存区的数据是否已经处理完毕
    irq_enable(IRQ0_SOUND);
    irq_install(IRQ0_SOUND,exception_handler_sound);
}

void do_handle_sound(exception_frame_t *frame){
    irq_send_eoi(IRQ0_SOUND); // 必须响应才声明能处理下一个了
}

void play_sound(int pos){
    wav_header_t *header= mem_alloc_page(15);
    read_disk(DISK_MASTER,pos,120,header);
    // 简单校验格式
    if(header->wave[0]!='w'||header->wave[1]!='a'||header->wave[2]!='v'||header->wave[3]!='e'){
        return;
    }
    if(header->bits==8&&header->channels==1){
        // 8位 单声道
    } else if(header->bits==16&&header->channels==2){
        // 16位 双声道
    } else{
        return;
    }
}