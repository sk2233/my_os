/* 指定程序入口 */
ENTRY(entry)
SECTIONS
{
    /* 设置到 2m 的位置 */
	. = 0x200000;
	.text : {
		*(*.text)
	}

	.rodata : {
		*(*.rodata)
	}

	.data : {
		*(*.data)
	}

	.bss : {
		*(*.bss)
	}
}
