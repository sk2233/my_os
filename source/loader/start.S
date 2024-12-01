  	// 16位代码，务必加上
  	.code16
 	.text
	.global _start
	.extern loader_entry
_start:
	// 调用boot_entry c 函数
	jmp loader_entry
