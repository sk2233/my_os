	#include "boot.h"

  	// 16位代码，务必加上
  	.code16
 	.text
	.global _start
	.extern boot_entry
_start:
	// 初始化段寄存器
	mov $0,%ax
	mov %ax,%ds
	mov %ax,%ss
	mov %ax,%es
	mov %ax,%fs
	mov %ax,%gs

	// 初始化栈指针
	mov $0x7c00,%esp

	// 调用bios 打印字符串   ah设置 al字符
	mov $0xe,%ah
	mov $'L',%al
	int $0x10

read_kernel:
	mov $0x8000, %bx	// 读取到的内存地址
	mov $0x2, %ah		// ah: 0x2读磁盘命令
	mov $0x2, %cx		// ch:磁道号，cl起始扇区号
	mov $64, %al		// al: 读取的扇区数量, 必须不大于128，这里使用64k作为kernel，若是kernel太大会出问题，可以尝试多读几次
	mov $0x0080, %dx	// dh: 磁头号，dl驱动器号0x80(磁盘1)
	int $0x13
	jc read_kernel      // 发生失败重复读取

	// 调用boot_entry c 函数
	jmp boot_entry

	jmp .
	
	// 引导结束段  boot_end 在链接器中指明了内存位置保证魔数位置正确
	.section boot_end, "ax"
boot_sig: .byte 0x55, 0xaa
