  	// 16位代码，务必加上
  	.code16
 	.text
	.global _start
	.extern loader_entry
_start:
	// 调用boot_entry c 函数
	jmp loader_entry

    .code32
    .text
    .global protect_mode_entry
    .extern load_kernel
protect_mode_entry:
    mov $16,%ax
    mov %ax,%ds
    mov %ax,%ss
    mov %ax,%es
    mov %ax,%fs
    mov %ax,%gs
    jmp $8,$load_kernel
    jmp .