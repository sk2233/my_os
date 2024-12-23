#ifndef FS_H
#define FS_H

#include "comm/types.h"

#define	DISK_CMD_IDENTIFY				0xEC	// IDENTIFY命令
#define	DISK_CMD_READ					0x24	// 读命令
#define	DISK_CMD_WRITE					0x34	// 写命令

#define DISK_MASTER     0xE0
#define DISK_SLAVE     0xF0

#define FS_INVALID  0x00      // 无效文件系统类型
// FAT16文件系统类型 的两种类型
#define FS_FAT16A  0x06
#define FS_FAT16B  0x0E

#define FAT_COUNT (1024*8)
#define FILE_COUNT  1024
// 一个簇多少扇区 4k  8扇区
#define FILE_CHUNK_SIZE 8
#define FILE_ITEM_INDEX 4
#define FILE_START_INDEX 8

// 0 1 已经被前面索引占用，不会被使用
#define FAT_EMPTY 0
#define FAT_END 1

// 真实 fat32 的表项目
//typedef struct file_item {
//    uint8_t DIR_Name[11];                   // 文件名
//    uint8_t DIR_Attr;                      // 属性
//    uint8_t DIR_NTRes;
//    uint8_t DIR_CrtTimeTeenth;             // 创建时间的毫秒
//    uint16_t DIR_CrtTime;         // 创建时间
//    uint16_t DIR_CrtDate;         // 创建日期
//    uint16_t DIR_LastAccDate;     // 最后访问日期
//    uint16_t DIR_FstClusHI;                // 簇号高16位
//    uint16_t DIR_WrtTime;         // 修改时间
//    uint16_t DIR_WrtDate;         // 修改时期
//    uint16_t DIR_FstClusL0;                // 簇号低16位
//    uint32_t DIR_FileSize;                 // 文件字节大小
//} file_item_t;

// 文件项目
typedef struct file_item { // 16 byte
    char name[10];
    uint16_t index; // 文件始簇号
    uint32_t size; // 文件大小
}file_item_t; // index = 0  标记文件不存在  name 为空标记后面就全部是空了

// 0～16k fat 表   16k~32k 文件项目   32k~  具体内容  下标从 8开始

typedef struct file{
    char name[10];
    uint16_t index; // 文件始簇号
    uint32_t size; // 文件大小
    int item_index; // 项目索引方便修改
    uint32_t pos; // 当前操作位置 默认 0
    uint16_t buff_index; // 缓存对应的簇号  默认 0 0是无效簇号
    void *buff; // 读取，写入缓存 一个簇 4k
}file_t;

#pragma pack(1)
// 位于 1 扇区标识前  有 4个  其前面是 boot 代码
typedef struct mbr_item { // 只使用  system_id   relative_sector   total_sector
    uint8_t boot_active;               // 分区是否活动
    uint8_t start_header;              // 起始header  会使用下面 relative_sector 与 total_sector
    uint16_t start_sector : 6;         // 起始扇区
    uint16_t start_cylinder : 10;	    // 起始磁道
    uint8_t system_id;	                // 文件系统类型  这个也是有用的
    uint8_t end_header;                // 结束header  会使用下面 relative_sector 与 total_sector
    uint16_t end_sector : 6;           // 结束扇区
    uint16_t end_cylinder : 10;        // 结束磁道
    uint32_t relative_sector;	        // 相对于该驱动器开始的相对扇区数
    uint32_t total_sector;            // 总的扇区数
}mbr_item_t;
#pragma pack()

void read_disk(uint8_t disk,uint32_t sector, uint16_t sector_count, void* buf);
void write_disk(uint8_t disk,uint32_t sector, uint16_t sector_count, void* buf);
void fs_init();
void fs_list();
void fs_flush(); // 写入元数据

file_t *file_open(const char *name,boot_t create);
file_t *file_create(const char *name);
int file_read(file_t *file,int size,void *buff);
int file_write(file_t *file,int size,void *buff);
void file_seek(file_t *file,int pos); // 只支持绝对位置
void file_delete(file_t *file); // 与file_close类似会清理文件结构
void file_flush(file_t *file);
void file_close(file_t *file);

#endif
