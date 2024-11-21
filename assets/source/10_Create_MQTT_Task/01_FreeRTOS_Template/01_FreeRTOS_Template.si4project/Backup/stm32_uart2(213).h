/* 文件名称：stm32_uart2.h
 * 摘要：    uart2 硬件驱动程序
 * 修改历史：2024.11.9  
 * 修改内容：创建文件
 *----------------------------------------
 * Version ：v01 	  
 * Author  ：Lizile	   
 */

#ifndef __STM32_UART2_H
#define __STM32_UART2_H

 /* 函数名  ： USART2_Lock_Init
  * 输入参数： 无
  * 输出参数： 无
  * 返回值  ： 无
  * 函数功能： 初始化串口 USART2 的互斥锁
  *--------------------------------------------------------------------------------
  * 最后修改： 2024.11.9
  * 修改内容： 创建函数
 */
extern void USART2_Lock_Init(void);

 /* 函数名  ： USART2_IRQHandler
  * 输入参数： 无
  * 输出参数： 无
  * 返回值  ： 无
  * 函数功能： USART2 的中断处理函数，如果发生 RXNE 中断，						  \
  * 		   则读取 ESP8266 接收到的数据，放入 USART2 的环形缓冲区 uart2_buffer
  *--------------------------------------------------------------------------------
  * 最后修改： 2024.11.7
  * 修改内容： 增加功能，释放互斥量唤醒 USART2_Read
 */
extern void USART2_IRQHandler(void);

/* 函数名  ： USART2_Read
 * 输入参数： c --> 从环形缓冲区读到的数据保存到此处
 *			  timeout --> 阻塞时间，如果读不到数据，愿意等待多久
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 从 USART2 的环形缓冲区 uart2_buffer 中读走 1 个字符，放入 c 地址，没有数据可读就阻塞
*/
extern void USART2_Read(char *c, int timeout);

/* 函数名  ： USART2_Write
 * 输入参数： buf --> 数据缓冲区，存放待发送的数据
 *            len --> 数据长度
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 使用死等（查询）方式，通过 USART2 将 buf 中长度为 len 的数据发送出去
*/
extern void USART2_Write(char *buf, int len);

#endif /* __STM32_UART2_H */

