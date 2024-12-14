## 前置准备文档
https://www.yuque.com/lishutong-docs/lines200/bwqcvq
## 启动流程
![img.png](img.png)
8086的启动流程，但是为了兼容现代cpu 也是这样的
## 启动扇区
![img_2.png](img_2.png)
会依次检查所有磁盘的 0 扇区是否为引导扇区
## 开发流程
![img_1.png](img_1.png)
## 段:偏移的访问方式
![img_3.png](img_3.png)
因为CPU地址线只有20位，为了实现32位需要段寄存器，除此之外段寄存器还有其他作用
## 实模式内存映射
![img_4.png](img_4.png)
实模式下只能访问1m以内的内存，只有灰色区域可以自由使用(Boot程序与Boot栈区也在灰色区域)
## 实模式
![img_5.png](img_5.png)
## 保护模式
![img_6.png](img_6.png)
## 保护模式切换流程
![img_7.png](img_7.png)
## elf数据存储
![img_9.png](img_9.png)
## 内存分段分页
![img_8.png](img_8.png)
## 段选择子
![img_10.png](img_10.png)
索引从3开始存储，低 3 位有其他用途
## 内存访问的过程
![img_11.png](img_11.png)
基地址+偏移地址=线性地址<br>
若是开启了分页，线性地址还需要再转换
## 中断处理
![img_12.png](img_12.png)
![img_13.png](img_13.png)
基地址在gdt表项中，偏移在idt表项中
## 中断保护
![img_14.png](img_14.png)
发生中断时cpu会自动压入栈中一些寄存器，还需要我们手动压入一些寄存器进行保护
## 中断处理参数传递
![img_15.png](img_15.png)
压入栈顶esp相当于上述参数结构体的指针(栈从高向低)
## TSS中各个字段
![img_16.png](img_16.png)
## 任务切换
![img_17.png](img_17.png)
## 基本锁结构
![img_18.png](img_18.png)