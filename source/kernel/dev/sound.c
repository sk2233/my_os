#include "dev/sound.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "core/fs.h"
#include "core/mem.h"
#include "tool/mem.h"

static boot_t sound_ready;

void exception_handler_sound();

void sound_out(uint8_t cmd){
    while (inb(SOUND_WRITE_PORT) & 128){} // 等待硬件不忙再写入
    outb(SOUND_WRITE_PORT, cmd);
}

void sound_init(){
    // 设置初始化
    outb(SOUND_RESET_PORT,0x01);
    outb(SOUND_RESET_PORT,0x00); // 一般需要等待 3ms 再发送但是因为虚拟机模拟无需等待
    // 等待初始化完成
    while ((inb(SOUND_STATE_PORT)&0x80)==0){}
    uint8_t data = inb(SOUND_READ_PORT); // 0xAA 表示准备就绪
    sound_ready=data==0xAA;
    sound_out(0xD1); // 打开声霸卡

    // 添加中断 判断缓存区的数据是否已经处理完毕
    irq_enable(IRQ0_SOUND);
    irq_install(IRQ0_SOUND,exception_handler_sound);
}

//==============DMA================

// 各通道触发器重置寄存器
static uint8_t DMA_RESET[] = {
        0x0C,
        0x0C,
        0x0C,
        0x0C,
        0xD8,
        0xD8,
        0xD8,
        0xD8,
};

// 重置 DMA
void dma_reset(uint8_t channel){
    uint16_t port = DMA_RESET[channel];
    outb(port, 0);
}

// 各通道掩码寄存器
static uint8_t DMA_MASK1[] = {
        0x0A,
        0x0A,
        0x0A,
        0x0A,
        0xD4,
        0xD4,
        0xD4,
        0xD4,
};

// 设置 DMA 掩码  是否开启 DMA
void dma_mask(uint8_t channel, boot_t mask){
    uint16_t port = DMA_MASK1[channel];
    uint8_t data = channel % 4;
    if (!mask){
        data |= 0x4;
    }
    outb(port, data);
}

// 各通道起始地址寄存器
static uint8_t DMA_ADDR[] = {
        0x00,
        0x02,
        0x04,
        0x06,
        0xC0,
        0xC4,
        0xC8,
        0xCC,
};

// 页地址寄存器
static uint8_t DMA_PAGE[] = {
        0,
        0x83,
        0x81,
        0x82,
        0,
        0x8B,
        0x89,
        0x8A,
};

// 设置起始地址
void dma_addr(uint8_t channel, void *addr){ // 地址 32 位只有  24位有效  地址最大只能是 24位 16m 以内
    uint16_t port = DMA_ADDR[channel];
    uint16_t offset = ((uint32_t)addr) % 0x10000; // 64k  先取低 16位
    if (channel > 4){
        offset >>= 1;
    }
    outb(port, offset & 0xFF);
    outb(port, (offset >> 8) & 0xFF);

    port = DMA_PAGE[channel]; // dma 一个页  64k
    outb(port, (uint32_t)addr >> 16); // 再取高 8 位
}

// 各通道计数寄存器
static uint8_t DMA_COUNT[] = {
        0x01,
        0x03,
        0x05,
        0x07,
        0xC2,
        0xC6,
        0xCA,
        0xCE,
};

// 设置传输大小
void dma_size(uint8_t channel, uint32_t size){
    uint16_t port = DMA_COUNT[channel];
    if (channel >= 5){
        size >>= 1;
    }
    outb(port, (size - 1) & 0xFF); // 因为 -1 最大只能传输  0x10000 64k
    outb(port, ((size - 1) >> 8) & 0xFF);
}

// 各通道模式寄存器
static uint8_t DMA_MODE[] = {
        0x0B,
        0x0B,
        0x0B,
        0x0B,
        0xD6,
        0xD6,
        0xD6,
        0xD6,
};

// 设置 DMA 模式
void dma_mode(uint8_t channel, uint8_t mode){
    uint16_t port = DMA_MODE[channel];
    outb(port, mode | (channel % 4));
}

//==============================

static uint8_t channel; // DMA 通道 一般有 8个
static void *buff;
static uint32_t buff_size;
static uint32_t sample_rate;

void write_data(){
    uint32_t size=0x10000; // 使用最大的 64k 缓存
    if(buff_size<size){
        size=buff_size;
    }
    buff_size-=size;
    // 设置 DMA 传输数据
    dma_mask(channel, FALSE); // 打开 DMA
    dma_addr(channel, buff); // 设置拷贝位置
    dma_size(channel, size); // 设置拷贝大小
    dma_mode(channel, 0x48); // 设置拷贝模式 固定 单次拷贝读内存的方式
    buff+=size; // 向后移动
    // 设置声霸卡
    sound_out(0x41);// 设置采样率
    sound_out((sample_rate >> 8) & 0xFF);
    sound_out(sample_rate & 0xFF);
    if(channel>4){  // 设置声道
        sound_out(0xB0);
        sound_out(0x30);
        size >>= 2; // size /= 4  双声道 16 位 要比单声道 8 位 大 4倍
    } else{
        sound_out(0xC0);
        sound_out(0x00);
    }// 设置写入数据大小
    sound_out((size - 1) & 0xFF); // -1 最大只能传输  64k
    sound_out(((size - 1) >> 8) & 0xFF);
    dma_mask(channel, TRUE); // 关闭 DMA
}

void do_handle_sound(exception_frame_t *frame){
    irq_send_eoi(IRQ0_SOUND); // 必须响应才声明能处理下一个了
    // 声霸卡中断响应  需要独立响应
    inb(0x22F);
    uint8_t state = inb(SOUND_STATE_PORT);
    if(buff_size>0){ // 还有数据继续拷贝
        write_data();
    }
}

void play_sound(int pos){
    // 读取 wave 文件
    wav_header_t *header= mem_alloc_page(300);
    read_disk(DISK_MASTER,pos,2400,header);
    // 简单校验格式
    if(header->wave[0]!='W'||header->wave[1]!='A'||header->wave[2]!='V'||header->wave[3]!='E'){
        return;
    }
    if(header->bits==8&&header->channels==1){
        // 8位 单声道
        channel=1; // DMA 通道 1 2 3  用于 8 位数据的传输  0 用于 DRAM 刷新不能使用
    } else if(header->bits==16&&header->channels==2){
        // 16位 双声道
        channel=5; // DMA 通过  5 6 7 用于 16 位数据的传输  4 用于连接从片不能使用
        // DMA 的传输内容必须是连续的
    } else{
        return;
    }

    // 初始化 dma 与要传输的参数
    dma_reset(channel);
    buff_size=header->chunk_size;
    sample_rate=header->sample_rate;
    buff=header+1;// 移除头部信息
//    mem_cpy(buff,header+1,buff_size); // 移除头部信息 并对齐
    write_data();
}