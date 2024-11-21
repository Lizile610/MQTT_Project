/* 文件名称：at_uart_hal.h
 * 摘要    ：AT 命令的 UART 硬件接口程序
 * 修改历史：2024.11.6  
 * 修改内容：增加读 ESP8266 返回信息的函数
 *----------------------------------------
 * Version ：v01 	  
 * Author  ：Lizile	   
 */

#ifndef __AT_UART_HAL_H
#define __AT_UART_HAL_H

/* 函数名  ： HAL_AT_Send
 * 输入参数： buf --> 要发送给 ESP8266 的 AT 命令或数据
 *            len --> 要发送的数据长度
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 选择不同的硬件接口向 ESP8266 发送 AT 命令，目前只有 UART2 接口
*/
extern void HAL_AT_Send(char *buf, int len);

/* 函数名  ： HAL_AT_Recv
 * 输入参数： c --> 从 ESP8266 返回的数据放在这个地址
 *            timeout --> 超时时间，当没有数据可以读时，愿意等待（阻塞）多久
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 读 ESP8266 模块接收到的数据，调用一次只读取一个字节，保存到 c 的地址，没有数据可读就阻塞
 */
extern void HAL_AT_Recv(char *c, int timeout);

#endif /* __AT_UART_HAL_H */
