/* 文件名称：ring_buffer.c
 * 摘要：
 *  
 * 修改历史      版本号      Author      修改内容
 *--------------------------------------------------
 * 2024.11.3      v01        Lizile      创建文件
 *--------------------------------------------------
*/

#include "ring_buffer.h"

/* 函数名  ： ring_buffer_init
 * 输入参数： dst_buf --> 指向目标环形缓冲区
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 初始化缓冲区
*/
void ring_buffer_init(ring_buffer *dst_buf)
{
	/* 读写位置初始化为 0 */
	dst_buf->pW = 0;  
	dst_buf->pR = 0;
}

/* 函数名  ： ring_buffer_write
 * 输入参数： dst_buf --> 指向目标环形缓冲区
 *            c --> 要写入的数据
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 向目标环形缓冲区写入一个字节的数据，如果缓冲区满，则丢弃此数据
*/
void ring_buffer_write(unsigned char c, ring_buffer *dst_buf)
{
	int pNextW = (dst_buf->pW + 1) % BUFFER_SIZE;  // 下一个写位置，如果越界则回到 0 位置
	if (pNextW != dst_buf->pR)
	{
		dst_buf->buffer[dst_buf->pW] = c;  // 将数据写到写位置
		dst_buf->pW = pNextW;              // 更新写位置
	}
}

/* 函数名  ： ring_buffer_read
 * 输入参数： dst_buf --> 指向目标环形缓冲区
 *            c --> 从目标环形缓冲区读到的数据保存到内存中哪个地址
 * 输出参数： 无
 * 返回值  ： 读取到数据，返回 0；否则，返回 -1
 * 函数功能： 从目标环形缓冲区读取一个字节的数据保存到内存中，    \
              如果目标环形缓冲区已空，返回 -1，表示读取失败
*/
int ring_buffer_read(unsigned char *c, ring_buffer *dst_buf)
{
	if (dst_buf->pR == dst_buf->pW)  // 如果读位置追上了写位置，说明环形缓冲区已满
		return -1;
	else
	{
		*c = dst_buf->buffer[dst_buf->pR];              // 将读位置的数据保存到 c 指向的内存
		dst_buf->pR = (dst_buf->pR + 1) % BUFFER_SIZE;  // 更新读位置，越界则回到 0 位置
		return 0;
	}
}

