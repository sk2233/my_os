#ifndef CONSOLE_H
#define CONSOLE_H

#include "comm/types.h"

#define CONSOLE_ADDR 0xb8000
#define CONSOLE_ROW 25
#define CONSOLE_COL 80

#define COLOR_BLACK			 0
#define        COLOR_BLUE			 1
#define       COLOR_GREEN			 2
#define       COLOR_CYAN			 3
#define        COLOR_RED			4
#define        COLOR_MAGENTA		 5
#define       COLOR_BROWN			 6
#define       COLOR_GRAY			 7
#define       COLOR_DARK_GRAY 	 8
#define       COLOR_LIGHT_BLUE	 9
#define        COLOR_LIGHT_GREEN	 10
#define        COLOR_LIGHT_CYAN	 11
#define        COLOR_LIGHT_RED		12
#define        COLOR_LIGHT_MAGENTA	 13
#define       COLOR_YELLOW		14
#define       COLOR_WHITE	 15

typedef union console_char{
    uint16_t v;
    struct {
        char ch;
        char fg:4;
        char bg:3;
        char flash:1;
    };
}console_char_t;

typedef struct console{
    int cursor;
    console_char_t *buff;
    char bg;
    char fg;
    char flash;
}console_t;

void console_init();
void console_write(const char *str,int len);
void console_clear();
void console_style(char bg,char fg,boot_t flash);

#endif
