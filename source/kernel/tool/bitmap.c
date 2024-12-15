#include "tool/bitmap.h"
#include "tool/mem.h"

void bitmap_init(bitmap_t *bitmap,uint8_t *mask,int count){
    mem_set(bitmap,0, sizeof(bitmap_t));
    bitmap->mask=mask;
    bitmap->count=count;
}

void bitmap_set(bitmap_t *bitmap,int index,boot_t val){
    if(index>= bitmap->count){
        return;
    }
    if(val){
        bitmap->mask[index/8]|=1<<(index%8);
    } else{
        bitmap->mask[index/8]&=~(1<<(index%8));
    }
}

boot_t bitmap_get(bitmap_t *bitmap,int index){
    if(index>= bitmap->count){
        return FALSE;
    }
    if((bitmap->mask[index/8] & (1<<(index%8)) )>0){
        return TRUE;
    } else{
        return FALSE;
    }
}

int bitmap_find_empty(bitmap_t *bitmap,int count){
    int size=0;
    for (int i = 0; i < bitmap->count; ++i) {
        if(bitmap_get(bitmap,i)){
            size=0;
        } else{
            size++;
            if(size>=count){
                return i-count+1;
            }
        }
    }
    return -1;
}