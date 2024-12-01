SECTIONS  {
    . = 0x10000; # 指定当前位置，后面数据会进行依次存放
    .text : {     # 收集项目下都所有 .text 放在一起 
        *(.text)  
    }
    .rodata : { 
        *(.rodata)
    }
    .data : { 
        *(.data)
    }
    .bss : { 
        *(.bss)
    }
}