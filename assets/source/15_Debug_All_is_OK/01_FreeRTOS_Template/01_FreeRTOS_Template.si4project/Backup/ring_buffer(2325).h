/* 文件名称：ring_buffer.h
 * 摘要：
 *  
 * 修改历史      版本号      Author      修改内容
 *--------------------------------------------------
 * 2024.11.3      v01        Lizile      创建文件
 *--------------------------------------------------
*/

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include "stm32f1xx_hal.h"   /* HAL库相关 */

#define BUFFER_SIZE  1024    /* 环形缓冲区的大小 */

/* 环形缓冲区结构体 */
typedef struct
{
	unsigned char buffer[BUFFER_SIZE];  /* 环形缓冲区空间 */
	volatile unsigned int pW;           /* 写位置 */
	volatile unsigned int pR;           /* 读位置 */
} ring_buffer;

/* 函数名  ： ring_buffer_init
 * 输入参数： dst_buf --> 指向目标环形缓冲区
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 初始化缓冲区
*/
extern void ring_buffer_init(ring_buffer *dst_buf);

/* 函数名  ： ring_buffer_write
 * 输入参数： dst_buf --> 指向目标环形缓冲区
 *            c --> 要写入的数据
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 向目标环形缓冲区写入一个字节的数据，如果缓冲区满，则丢弃此数据
*/
extern void ring_buffer_write(unsigned char c, ring_buffer *dst_buf);

/* 函数名  ： ring_buffer_read
 * 输入参数： dst_buf --> 指向目标环形缓冲区
 *            c --> 从目标环形缓冲区读到的数据保存到内存中哪个地址
 * 输出参数： 无
 * 返回值  ： 读取到数据，返回 0；否则，返回 -1
 * 函数功能： 从目标环形缓冲区读取一个字节的数据保存到内存中，    \
              如果目标环形缓冲区已空，返回 -1，表示读取失败
*/
extern int ring_buffer_read(unsigned char *c, ring_buffer *dst_buf);

#endif /* __RING_BUFFER_H */

这是一个示例    这是另一个示例







