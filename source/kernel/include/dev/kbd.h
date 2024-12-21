#ifndef KBD_H
#define KBD_H

#define KBD_PORT_DATA			0x60
#define KBD_PORT_STAT			0x64

#define KBD_STAT_READY		(1 << 0)

void kbd_init();

void exception_handler_kbd();

#endif