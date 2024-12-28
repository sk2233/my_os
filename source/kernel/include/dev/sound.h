#ifndef SOUND_H
#define SOUND_H

#include "comm/types.h"

#define SOUND_RESET_PORT 0x226
#define SOUND_READ_PORT 0x22A
#define SOUND_WRITE_PORT 0x22C
#define SOUND_STATE_PORT 0x22E

#pragma pack(1)
typedef struct wav_header{
    char riff[4];       // RIFF string  魔数
    uint32_t chunk_size;    // 文件长度
    char wave[4];       // WAVE string  魔数
    char fmt[4];        // 'fmt ' string  标志格式信息开始
    uint32_t format;       // 本块数据的大小，(对于PCM, 值为16)
    uint16_t type;         // 格式 1-PCM, 3- IEEE float ...
    uint16_t channels;     // 通道数
    uint32_t sample_rate;  // 采样率 sampling rate (blocks per second)
    uint32_t byte_rate;     // 码率 SampleRate * NumChannels * BitsPerSample/8
    uint16_t block_align;  // NumChannels * BitsPerSample/8 数据块大小
    uint16_t bits;         // 位数 8 bits, 16 bits
    char  data[4];   // 'data' 标志数据卡死
    uint32_t data_size; // 数据大小
}wav_header_t;
#pragma pack()

void sound_init();
void play_sound(int pos);

#endif
