#ifndef FS_H
#define FS_H

#include "comm/types.h"

void read_disk(uint32_t sector, uint16_t sector_count, void* buf);

#endif
