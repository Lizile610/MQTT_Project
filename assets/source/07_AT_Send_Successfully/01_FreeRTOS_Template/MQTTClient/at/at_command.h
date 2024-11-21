/* 文件名称：at_command.h
 * 摘要：    AT 命令函数头文件
 * 修改历史：2024.11.8  
 * 修改内容：增加网络数据的收包函数
 *----------------------------------------
 * Version ：v01 	  
 * Author  ：Lizile	   
 */

#ifndef __AT_COMMAND_H
#define __AT_COMMAND_H

#define AT_OK		 0       // AT 命令响应的状态为 OK
#define AT_ERR		-1       // AT 命令响应的状态为 ERROR
#define AT_TIMEOUT	-2       // AT 命令超时无响应

/* 函数名  ： AT_Init
 * 输入参数： 无
 * 输出参数： 无
 * 返回值  ： 
 * 函数功能： 初始化 AT 命令相关内容，如互斥量
 */
extern int AT_Init(void);

/* 函数名  ： ATSendCmd
 * 输入参数： buf --> AT 命令
 *            len --> 命令的数据长度
 *			  resp --> 保存 AT 命令的响应信息
 *            resp_len --> 响应信息的数据长度
 *            timeout --> 超时时间，单位为 tick
 * 输出参数： 无
 * 返回值  ： 
 * 函数功能： 通用的 AT 命令发送函数
 *--------------------------------------
 * eg.	buf = "AT+CIPMODE=1"
 *		len = strlen(buf);
 *		timeout : tick
*/
extern int ATSendCmd(char *buf, int len, char *resp, int resp_len, int timeout);

/* 函数名  ： ATReadPacket
 * 输入参数： buf -> 暂存网络数据的缓冲区
 *            len -> 期望读多长的网络数据到 buf
 *            *read_len -> 实际读到了多少网络数据到 buf
 *            timeout -> 超时时间，如果读不到网络数据，愿意等待多久
 * 输出参数： 无
 * 返回值  ： 是否读取成功
 * 函数功能： 读网络数据包
 *--------------------------------------
*/
extern int ATReadPacket(char *buf, int len, int *read_len, int timeout);

/* 函数名  ： ATRecvParser
 * 输入参数： params -> 参数的地址，一般是结构体指针，内含多个参数
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 解析任务函数，解析发送 AT 命令后接收到的响应以及 “不请自来的” 网络数据包
 *--------------------------------------
*/
extern void ATRecvParser(void *params);

/* AT 命令的测试任务 */
extern void Task_ATTest(void *params);

#endif /* __AT_COMMAND_H */

