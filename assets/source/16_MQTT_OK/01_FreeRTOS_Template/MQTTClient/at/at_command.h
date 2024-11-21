/* 文件名称：at_command.h
 * 摘要：    AT 命令函数头文件
 * 修改历史：2024.11.11 
 * 修改内容：修改收包函数
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

/* 函数名  ： ATSendData
 * 输入参数： buf --> 要向服务器发送的数据
 *            len --> 数据长度
 *            timeout --> 超时时间，单位为 tick
 * 输出参数： 无
 * 返回值  ： 本次发送数据得到的响应状态
 * 函数功能： 通用的 AT 数据发送函数
 *--------------------------------------------------------------------
 * 修改时间： 2024.11.11
 * 修改内容： 创建函数
 */
extern int ATSendData(unsigned char *buf, int len, int timeout);

/* 函数名  ： ATSendCmd
 * 输入参数： buf --> AT 命令     eg. buf = "AT+CIPMODE=1"
 *			  resp --> 保存 AT 命令的响应信息，不需要保存时传入 NULL
 *            resp_len --> 希望得到多长的返回信息，不需要保存时传入 0
 *            timeout --> 超时时间，单位为 tick
 * 输出参数： 无
 * 返回值  ： 
 * 函数功能： 通用的 AT 命令发送函数
 *--------------------------------------------------------------------
 * 修改时间： 2024.11.10
 * 修改内容： 删除参数 len
 */
extern int ATSendCmd(char *buf, char *resp, int resp_len, int timeout);

/* 函数名  ： ATReadData
 * 输入参数： buf -> 从网络数据包的缓冲区读到的数据放入这里
 *            timeout --> 超时时间，愿意等待多久
 * 输出参数： 无
 * 返回值  ： AT_OK : 读取成功
 *            AT_TIMEOUT : 超时，读取失败
 * 函数功能： 从网络数据包的环形缓冲区中读取新的网络数据（一次一个字节）
 *--------------------------------------
 * 修改时间： 2024.11.11
 * 修改内容： 创建函数
*/
extern int ATReadData(unsigned char *c, int timeout);

/* 函数名  ： ATRecvParser
 * 输入参数： params -> 参数的地址，一般是结构体指针，内含多个参数
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 解析任务函数，解析发送 AT 命令后接收到的响应以及 “不请自来的” 网络数据包
 *--------------------------------------
 * 修改时间：2024.11.11
 * 修改内容：改为将网络数据放入环形缓冲区
*/
extern void ATRecvParser(void *params);

/* AT 命令的测试任务 */
extern void MQTT_Client_Task(void *params);

#endif /* __AT_COMMAND_H */

