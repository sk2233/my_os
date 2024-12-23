#include "dev/console.h"
#include "tool/mem.h"
#include "comm/cpu_instr.h"

static console_t console; // 这里暂时只使用一个 tty

void console_init(){
    mem_set(&console,0, sizeof(console_t));
    console.buff=(console_char_t *)CONSOLE_ADDR;
    console.bg=COLOR_BLACK;
    console.fg=COLOR_WHITE;
}

void write_char(char ch){
    console_char_t *temp= console.buff+console.cursor;
    temp->ch=ch;
    temp->bg=console.bg;
    temp->fg=console.fg;
    temp->flash=console.flash;
    console.cursor++;
}

void scroll_up(int line){
    for (int i = 0; i <CONSOLE_ROW-line; ++i) {
        mem_cpy(console.buff+i*CONSOLE_COL,console.buff+(i+line)*CONSOLE_COL, sizeof(console_char_t)*CONSOLE_COL);
    }
    for (int i = 0; i < line; ++i) {
        mem_set(console.buff+(CONSOLE_ROW-i-1)*CONSOLE_COL,0, sizeof(console_char_t)*CONSOLE_COL);
    }
    console.cursor-=CONSOLE_COL;
}

void update_cursor(){
    outb(0x3D4, 0x0E);		// 写高地址
    outb(0x3D5, (uint8_t) ((console.cursor >> 8) & 0xFF));
    outb(0x3D4, 0x0F);		// 写低地址
    outb(0x3D5, (uint8_t) (console.cursor & 0xFF));
}

void console_base(int base){ // 设置显示的初始位置
    uint16_t pos=base*CONSOLE_COL;
    outb(0x3D4, 0x0C);		// 写高地址
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
    outb(0x3D4, 0x0D);		// 写低地址
    outb(0x3D5, (uint8_t) (pos & 0xFF));
}

void console_write(const char *str,int len){
    char ch;
    while (((ch=*str)!='\0')&&(len>0)){
        switch (ch) {
        case '\n':
            console.cursor=(console.cursor/CONSOLE_COL+1)*CONSOLE_COL;
            break;
        default:
            write_char(ch);
            break;
        }
        if(console.cursor>=CONSOLE_COL*CONSOLE_ROW){
            scroll_up(1); // 暂时每次只会 1 行
        }
        str++;
        len--;
    }
    update_cursor(); // 更新光标位置
}

void console_clear(){
    mem_set(console.buff,0, sizeof(console_char_t)*CONSOLE_COL*CONSOLE_ROW);
}

void console_style(char bg,char fg,boot_t flash){
    console.bg=bg;
    console.fg=fg;
    console.flash=(char)flash;
}