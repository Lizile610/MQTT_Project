/* 文件名称：stm32_uart2.c
 * 摘要：    uart2 硬件驱动程序
 * 修改历史：2024.11.9  
 * 修改内容：创建文件
 *----------------------------------------
 * Version ：v01 	  
 * Author  ：Lizile	   
 */

#include "usart.h"
#include "ring_buffer.h"     /* 环形缓冲区函数相关头文件 */
#include "stm32_uart2.h"
#include "platform_mutex.h"  /* 平台互斥量函数相关头文件 */

static ring_buffer uart2_buffer;	  /* uart2 的环形 buffer */
static platform_mutex_t uart2_recv_mutex;  // 通知某些任务“USART2 接收到数据”的互斥量

/* 函数名  ： USART2_Lock_Init
 * 输入参数： 无
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 初始化串口 USART2 的互斥锁
 *--------------------------------------------------------------------------------
 * 最后修改： 2024.11.9
 * 修改内容： 将 uart2_buffer 的初始化移动到这里
*/
void USART2_Lock_Init(void)
{
	platform_mutex_init(&uart2_recv_mutex);  // 创建 UART2 的互斥锁，通知某些线程 UART2 接收到了数据（有数据可读）
	platform_mutex_lock(&uart2_recv_mutex);  // 先上锁，只有当 USART2 触发了接收中断才释放互斥锁

	ring_buffer_init(&uart2_buffer);	     /* 初始化 uart2 的环形 buffer */
}

/* 函数名  ： USART2_IRQHandler
 * 输入参数： 无
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： USART2 的中断处理函数，如果发生 RXNE 中断，                        \
 *            则读取 ESP8266 接收到的数据，放入 USART2 的环形缓冲区 uart2_buffer
 *--------------------------------------------------------------------------------
 * 最后修改： 2024.11.7
 * 修改内容： 增加功能，释放互斥量唤醒 USART2_Read
*/
void USART2_IRQHandler(void)
{
	uint32_t isrflags	= READ_REG(huart2.Instance->SR);   // 读 UART2 的状态寄存器 SR
	uint32_t cr1its 	= READ_REG(huart2.Instance->CR1);  // 读 UART2 的控制寄存器 CR1
	
	/* 如果发生的是 RXNE 中断
	 * 把数据读出来，存入环形buffer
	*/	
	char c;  // 保存从 UART2 读出来的数据

    if (((huart2.Instance->SR & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
		/* 如果使能了 RXNE 中断，并且发生了 RXNE 中断  
		 * 那么将 UART2 的数据寄存器 DR 中的数据读到 UART2 的环形 buffer 
		*/
      	c = huart2.Instance->DR;
      	ring_buffer_write(c, &uart2_buffer);

		platform_mutex_unlock_fromISR(&uart2_recv_mutex);  // 释放互斥量，通知某些任务 USART2 接收到了数据，环形缓冲区有数据可读
      	return;
    }
}

/* 函数名  ： USART2_Read
 * 输入参数： c --> 从环形缓冲区读到的数据保存到此处
 *			  timeout --> 阻塞时间，如果读不到数据，愿意等待多久
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 从 USART2 的环形缓冲区 uart2_buffer 中读走 1 个字符，放入 c 地址，没有数据可读就阻塞
*/
void USART2_Read(char *c, int timeout)
{
	while (1)
	{
		if (0 == ring_buffer_read((unsigned char *)c, &uart2_buffer))  
			return;  // 如果读到了数据就马上返回
		else
		{
			/* 如果读不到数据就阻塞，使用互斥量来唤醒
			 * 这个互斥量由 USART2 的接收中断来释放
			 */ 
			 platform_mutex_lock_timeout(&uart2_recv_mutex, timeout);
		}
	}
}

/* 函数名  ： USART2_Write
 * 输入参数： buf --> 数据缓冲区，存放待发送的数据
 *            len --> 数据长度
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 使用死等（查询）方式，通过 USART2 将 buf 中长度为 len 的数据发送出去
*/
void USART2_Write(char *buf, int len)
{
	int i = 0;
	while (i < len)
	{
		/* 如果 SR 的 TXE 位为 0，说明 TDR 非空，死等；
		 * 直到 TXE 的值为 1，说明 TDR 中的数据已经转移到移位寄存器中，可以发送下一帧数据
		 */		
		while ((huart2.Instance->SR & USART_SR_TXE) == 0);
		huart2.Instance->DR = buf[i];
		i++;
	}
}

