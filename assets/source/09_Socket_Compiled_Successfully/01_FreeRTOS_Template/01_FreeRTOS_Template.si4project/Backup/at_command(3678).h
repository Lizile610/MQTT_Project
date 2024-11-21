/* 文件名称：at_command.h
 * 摘要：    AT 命令函数头文件
 *  
 * 修改历史      版本号      Author      修改内容
 *--------------------------------------------------
 * 2024.11.7      v01        Lizile      修改文件
 *--------------------------------------------------
*/

#ifndef __AT_COMMAND_H
#define __AT_COMMAND_H

#define AT_OK		 0       // AT 命令响应的状态为 OK
#define AT_ERR		-1       // AT 命令响应的状态为 ERROR
#define AT_TIMEOUT	-2       // AT 命令超时无响应

/* 函数名  ： SetATStatus
 * 输入参数： status --> AT 命令响应的状态
 *              AT_OK、AT_ERR、AT_TIMEOUT 
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 设置 AT 命令响应的状态，OK 还是 ERROR 还是超时？
 */
extern void SetATStatus(int status);

/* 函数名  ： GetATStatus
 * 输入参数： 无
 * 输出参数： 无
 * 返回值  ： 
 * 函数功能： 获得 AT 命令响应的状态，OK 还是 ERROR 还是超时？
 */
extern int GetATStatus(void);

/* 函数名  ： AT_Init
 * 输入参数： 无
 * 输出参数： 无
 * 返回值  ： 
 * 函数功能： 初始化 AT 命令相关内容
 */
extern int AT_Init(void);

/* 函数名  ： ATSendCmd
 * 输入参数： buf --> AT 命令
 *            len --> 命令的数据长度
 *			  resp --> 保存 AT 命令的响应信息
 *            resp len --> 响应信息的数据长度
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

#endif /* __AT_COMMAND_H */

