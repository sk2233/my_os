#include "core/fs.h"
#include "comm/cpu_instr.h"
#include "comm/comm.h"
#include "tool/mem.h"
#include "core/mem.h"
#include "dev/console.h"

static uint16_t *fats;
static file_item_t *file_items;

void read_disk(uint8_t disk,uint32_t sector, uint16_t sector_count, void* buf) {
    outb(0x1F6, disk);
    // 读取扇区数量
    outb(0x1F2, (uint8_t) (sector_count >> 8));
    // 扇区号  48bit 我们只用  32bit
    outb(0x1F3, (uint8_t) (sector >> 24));		// LBA参数的24~31位
    outb(0x1F4, (uint8_t) (0));					// LBA参数的32~39位
    outb(0x1F5, (uint8_t) (0));					// LBA参数的40~47位
    // 读取扇区数量
    outb(0x1F2, (uint8_t) (sector_count));
    // 扇区号  48bit 我们只用  32bit
    outb(0x1F3, (uint8_t) (sector));			// LBA参数的0~7位
    outb(0x1F4, (uint8_t) (sector >> 8));		// LBA参数的8~15位
    outb(0x1F5, (uint8_t) (sector >> 16));		// LBA参数的16~23位
    //  读取命令
    outb(0x1F7, (uint8_t) DISK_CMD_READ);
    // 读取数据
    uint16_t *data_buf = (uint16_t*) buf;
    while (sector_count-- > 0) {
        // 每次扇区读之前都要检查，等待数据就绪
        while (inb(0x1F7) & 0x80) {}
        // 读取并将数据写入到缓存中
        for (int i = 0; i < SECTOR_SIZE / 2; i++) {
            *data_buf++ = inw(0x1F0);
        }
    }
}

void write_disk(uint8_t disk,uint32_t sector, uint16_t sector_count, void* buf){
    outb(0x1F6, disk);
    // 写入扇区数量
    outb(0x1F2, (uint8_t) (sector_count >> 8));
    // 扇区号  48bit 我们只用  32bit
    outb(0x1F3, (uint8_t) (sector >> 24));		// LBA参数的24~31位
    outb(0x1F4, (uint8_t) (0));					// LBA参数的32~39位
    outb(0x1F5, (uint8_t) (0));					// LBA参数的40~47位
    // 写入扇区数量
    outb(0x1F2, (uint8_t) (sector_count));
    // 扇区号  48bit 我们只用  32bit
    outb(0x1F3, (uint8_t) (sector));			// LBA参数的0~7位
    outb(0x1F4, (uint8_t) (sector >> 8));		// LBA参数的8~15位
    outb(0x1F5, (uint8_t) (sector >> 16));		// LBA参数的16~23位
    //  写入命令
    outb(0x1F7, (uint8_t) DISK_CMD_WRITE);
    // 写入数据
    uint16_t *data_buf = (uint16_t*) buf;
    while (sector_count-- > 0) {
        // 写入数据
        for (int i = 0; i < SECTOR_SIZE / 2; i++) {
            outw(0x1F0,*data_buf++);
        }
        // 每次扇区写入之后都要检查，等待数据就绪
        while (inb(0x1F7) & 0x80) {}
    }
}

void identify_disk(uint8_t disk){
    outb(0x1F6, disk); // 主磁盘
    //  识别命令
    outb(0x1F7, (uint8_t) DISK_CMD_IDENTIFY);
    if(!inb(0x1F7)){
        // 扇区不存在
        return;
    }
    // 等待数据
    while ((inb(0x1F7) & 0x88) != 0x8) {}
    uint16_t data_buf[256]; // 读取数据  会读取到各个信息，但是这里只使用大小信息
    for (int i = 0; i < SECTOR_SIZE / 2; i++) {
        data_buf[i]=inw(0x1F0);
    } // 实际使用的 uint64 存储的大小 但是这里使用 uint32 接受也是够用的
    uint32_t count=*(uint32_t *)(data_buf+100);
}

void detect_mbr_info(uint8_t disk){
    uint8_t data_buf[SECTOR_SIZE];
    read_disk(disk,0,1,data_buf);
    mbr_item_t *mbr_item=(mbr_item_t *)(data_buf+SECTOR_SIZE-2-4* sizeof(mbr_item_t));
}

void fs_reset(){
    uint8_t data_buf[SECTOR_SIZE];
    mem_set(data_buf,0,SECTOR_SIZE);
    // 只清空 fat 表与文件项目即可
    for (int i = 0; i < FILE_START_INDEX*FILE_CHUNK_SIZE; ++i) {
        write_disk(DISK_SLAVE,i,1,data_buf);
    }
}

void fs_init(){
    // 查看磁盘信息
    identify_disk(DISK_MASTER);
    identify_disk(DISK_SLAVE);
    detect_mbr_info(DISK_MASTER);
    detect_mbr_info(DISK_SLAVE);
    // 清空磁盘信息
//    fs_reset();
    // 文件表 fat 表信息先读入内存
    fats= mem_alloc(FAT_COUNT*2);
    file_items= mem_alloc(FILE_COUNT* sizeof(file_item_t));
    read_disk(DISK_SLAVE,0,FILE_ITEM_INDEX*FILE_CHUNK_SIZE,fats);
    read_disk(DISK_SLAVE,FILE_ITEM_INDEX*FILE_CHUNK_SIZE,(FILE_START_INDEX-FILE_ITEM_INDEX)*FILE_CHUNK_SIZE,file_items);
    // 测试代码
//    file_create("test");
//    fs_list();
//    fs_flush();
    //file_t *file=file_open("test",FALSE);
//    file_write(file,12,"Hello World!");
//    file_close(file);
//    fs_flush();
    //char buff[32];
    // file_read(file,12,buff);
}

// 写入元数据
void fs_flush(){
    write_disk(DISK_SLAVE,0,FILE_ITEM_INDEX*FILE_CHUNK_SIZE,fats);
    write_disk(DISK_SLAVE,FILE_ITEM_INDEX*FILE_CHUNK_SIZE,(FILE_START_INDEX-FILE_ITEM_INDEX)*FILE_CHUNK_SIZE,file_items);
}

void fs_list(){
    for (int i = 0; i < FILE_COUNT; ++i) {
        file_item_t *item=file_items+i;
        if(item->name[0]=='\0'){ // 后面都是 空
            break;
        }
        if(item->index>0){ // index > 0 才是有效的
            console_write(item->name,10);
            console_write("\n",1);
        }
    }
}

file_t *new_file(const char *name,uint16_t index,uint32_t size,int item_index){
    file_t *res= mem_alloc(sizeof(file_t));
    mem_cpy(res->name,(void *)name,10);
    res->index=index;
    res->size=size;
    res->item_index=item_index;
    res->buff= mem_alloc_page(1);
    return res;
}

file_t *file_open(const char *name,boot_t create){
    int len =str_len(name);
    for (int i = 0; i < FILE_COUNT; ++i) {
        file_item_t *item=file_items+i;
        if(item->name[0]=='\0'){ // 后面都是 空
            break;
        }
        if(item->index>0&& mem_eq((void *)name,item->name,len)){
            return new_file(name,item->index,item->size,i);
        }
    }
    if(create){
        return file_create(name);
    }
    return NULL;
}

uint16_t fat_find_index(){
    for (int i = 0; i < FAT_COUNT; ++i) {
        if(fats[i]==FAT_EMPTY){
            fats[i]=FAT_END; // 默认设置为末尾
            return i+FILE_START_INDEX;
        }
    }
    return 0; // 不太可能用完
}

uint16_t fat_size_index(uint16_t index,uint32_t size){
    if(size<FILE_CHUNK_SIZE*SECTOR_SIZE){
        return index;
    }
    return fat_size_index(fats[index-FILE_START_INDEX],size-FILE_CHUNK_SIZE*SECTOR_SIZE);
}

void fat_extend_index(uint16_t index,uint32_t size){
    if(size<FILE_CHUNK_SIZE*SECTOR_SIZE){
        return;
    }
    if(fats[index]==FAT_END){ // 还不能结束
        fats[index]=fat_find_index();
    }
    fat_extend_index(fats[index],size-FILE_CHUNK_SIZE*SECTOR_SIZE);
}

void fat_clear_index(int index){
    int next= fats[index-FILE_START_INDEX];
    if(next!=FAT_END){
        fat_clear_index(next);
    }
    fats[index-FILE_START_INDEX]=FAT_EMPTY;
}

file_t *file_create(const char *name){
    int len = str_len(name);
    // 先看看有没有，有的话直接返回
    for (int i = 0; i < FILE_COUNT; ++i) {
        file_item_t *item=file_items+i;
        if(item->name[0]=='\0'){ // 后面都是 空
            break;
        }
        if(item->index>0&& mem_eq((void *)name,item->name,len)){
            return new_file(name,item->index,item->size,i);
        }
    }
    // 没有进行创建
    for (int i = 0; i < FILE_COUNT; ++i) {
        file_item_t *item=file_items+i;
        if(item->index==0){
            mem_cpy(item->name,(void *)name,10);
            item->index=fat_find_index();
            item->size=0; // 没有写入最终要调用  fs_flush
            return new_file(name,item->index,item->size,i);
        }
    }
    return NULL; // 没有空间了
}

void load_buff(file_t *file){
    uint16_t index = fat_size_index(file->index,file->pos);
    if(index==file->buff_index){ // 已经缓存了
        return;
    }
    file->buff_index=index;
    read_disk(DISK_SLAVE,index*FILE_CHUNK_SIZE,FILE_CHUNK_SIZE,file->buff);
}

int file_read(file_t *file,int size,void *buff){
    if(file->pos+size>file->size){
        size=(int)(file->size-file->pos);
    }
    int need_size=size;
    while (need_size>0){
        load_buff(file);
        int pos=(int )(file->pos%(FILE_CHUNK_SIZE*SECTOR_SIZE));
        int has_size=FILE_CHUNK_SIZE*SECTOR_SIZE-pos;
        if(has_size<need_size){ // 读取完还不够
            mem_cpy(buff,file->buff+pos,has_size);
            buff+=has_size;
            file->pos+=has_size;
            need_size-=has_size;
        } else{ // 正好读取完毕
            mem_cpy(buff,file->buff+pos,need_size);
            buff+=need_size;
            file->pos+=need_size;
            need_size=0;
        }
    }
    return size; // 实际读取的大小
}

void file_flush(file_t *file){
    if(file->buff_index){
        write_disk(DISK_SLAVE,file->buff_index*FILE_CHUNK_SIZE,FILE_CHUNK_SIZE,file->buff);
    }
}

int file_write(file_t *file,int size,void *buff){
    if(file->pos+size>file->size){
        file->size=file->pos+size;
        file_item_t *item=file_items+file->item_index;
        item->size=file->size;
        fat_extend_index(file->index,file->size);
    }
    while (size>0){
        load_buff(file);
        int pos=(int )(file->pos%(FILE_CHUNK_SIZE*SECTOR_SIZE));
        int has_size=FILE_CHUNK_SIZE*SECTOR_SIZE-pos;
        if(has_size<size){
            mem_cpy(file->buff+pos,buff,has_size);
            file->pos+=has_size;
            buff+=has_size;
            size-=has_size;
            file_flush(file); // 主动刷入数据
        } else{
            mem_cpy(file->buff+pos,buff,size); // 写入量较小不主动刷入
            file->pos+=size;
            buff+=size;
            size=0;
        }
    }
    return (int)file->size; // 返回具体文件大小
}

void file_seek(file_t *file,int pos){
    if(pos>file->size){ // 这些数据都没有立即写入 需要在合适的位置  flush
        file->size=pos;
        file_item_t *item=file_items+file->item_index;
        item->size=file->size;
        fat_extend_index(file->index,file->size);
    }
    file->pos=pos;
}

void file_close(file_t *file){
    file_flush(file);
    mem_free(file->buff);
    mem_free(file);
}


void file_delete(file_t *file){
    file_item_t *item=file_items+file->item_index;
    item->index=0; // 清除 表项
    fat_clear_index(file->index); // 清除链接关系
    mem_free(file->buff); // 释放内存
    mem_free(file);
}