#include "main.h"
#include "comm/types.h"

void test(){
    int i=10;
    for (int j = 0; j < i; ++j) {
        i++;
    }
    for (;;) {
        i++;
    }
}

void entry(uint32_t arg){
    test();
}