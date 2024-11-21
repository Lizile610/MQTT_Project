/* 文件名称：at_command.c
 * 摘要：    AT 命令函数
 *  
 * 修改历史      版本号      Author      修改内容
 *--------------------------------------------------
 * 2024.11.6      v01        Lizile      创建文件
 *--------------------------------------------------
*/

#include "platform_mutex.h"  /* 平台互斥量相关函数 */
#include "at_command.h"		 	

#define AT_CMD_TIMEOUT 1000  /* 等待 AT 命令返回结果的超时时间 */
#define AT_RESP_LEN    100   /* g_at_resp 的 buffer 大小 */

static platform_mutex_t at_ret_mutex;  // 等待 AT 命令返回结果的信号量
static int g_at_status;  			   // AT 命令返回结果的状态
static char g_at_resp[AT_RESP_LEN];    // 全局buffer，保存解析线程中传递过来的解析好的数据

/* 函数名  ： SetATStatus
 * 输入参数： status --> AT 命令返回结果的状态
 *              AT_OK、AT_ERR、AT_TIMEOUT 
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 设置 AT 命令返回结果的状态，OK 还是 ERROR 还是超时？
 */
void SetATStatus(int status)
{
	g_at_status = status;
}

/* 函数名  ： GetATStatus
 * 输入参数： 无
 * 输出参数： 无
 * 返回值  ： 
 * 函数功能： 获得 AT 命令返回结果的状态，OK 还是 ERROR 还是超时？
 */
int GetATStatus(void)
{
	return g_at_status;
}

/* 函数名  ： AT_Init
 * 输入参数： 无
 * 输出参数： 无
 * 返回值  ： 
 * 函数功能： 初始化 AT 命令相关内容
 */
int AT_Init(void)
{
	platform_mutex_init(&at_ret_mutex);  // 创建等待 AT 命令返回信息的信号量，初始为 1
	platform_mutex_lock(&at_ret_mutex);  // 先上锁(让mutex=0)，只有当解析线程解锁(mutex=1)，才能唤醒发送线程
}

/* 函数名  ： ATSendCmd
 * 输入参数： buf --> AT 命令
 *            len --> 命令的数据长度
 *			  resp --> 保存 AT 命令的返回信息
 *            resp len --> 返回信息的数据长度
 *            timeout --> 超时时间，单位为 tick
 * 输出参数： 无
 * 返回值  ： 
 * 函数功能： 通用的 AT 命令发送函数
 *--------------------------------------
 * eg.	buf = "AT+CIPMODE=1"
 *		len = strlen(buf);
 *		timeout : tick
*/
int ATSendCmd(char *buf, int len, char *resp, int resp_len, int timeout)
{
	int ret;
	int err;

	/* 发送 AT 命令 */
	HAL_AT_Send(buf, len);
	HAL_AT_Send("\r\n", 2);
	
	/* 等待 AT 命令的返回结果
	 * ret : 
	 *      1 - 成功得到信号量，AT 命令的返回状态可能为 AT_OK 或 AT_ERR
	 * 	    0 - 超时返回，AT 命令的返回状态为 AT_TIMEOUT
	 */
	ret = platform_mutex_lock_timeout(&at_ret_mutex, AT_CMD_TIMEOUT);  // 获得信号量，并且让信号量 -1

	/* 判断 AT 命令的返回结果 */
	if (ret)
	{
		/* 如果有返回，将 AT 命令的返回信息存入 resp */
		err = GetATStatus();  // 获得 AT 命令返回结果的状态
		
		if (!err && resp)
		{
			/* 如果 AT 命令返回 OK，则将缓冲区 g_at_resp 中解析好的数据（长度为 resp_len）存入 resp 
		     * 注意不要越界
		     */
			memcpy(resp, g_at_resp, resp_len > AT_RESP_LEN ? AT_RESP_LEN : resp_len);  
		}
		return err;         // 返回 AT 命令返回结果的状态，可能为 AT_OK 或 AT_ERR
	}
	else
	{
		return AT_TIMEOUT;  // 超时返回
	}
}

/* 函数名  ： ATRecvParser
 * 输入参数： params -> 参数的地址，一般是结构体指针，内含多个参数
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 解析任务函数，解析发送 AT 命令后接收到的响应以及 “不请自来的” 网络数据包
 *--------------------------------------
*/
void ATRecvParser(void *params)
{	
	char buf[AT_RESP_LEN];  // 从 ESP8266 模块读到的数据保存在这个 buffer
	int i;

	while (1)
	{
		/* 逐个字节读取从 ESP8266 模块返回来的数据，放到 buf 中
		 * 使用阻塞的方式，阻塞时间设置为最大，如果 ESP8266 没有返回来数据，就一直等待
		 */
		HAL_AT_Recv(&buf[i], portMAX_DELAY);

		/* 解析数据：
		 * 每当从 ESP8266 返回来的数据中读到 "\r\n" 时，开始解析数据并保存解析好的数据；
		 * 否则进入下一轮循环
		 */
		if (i && (buf[i-1] == '\r') && (buf[i] == '\n'))
		{
			/* 如果碰到了回车换行，根据以下三种情况判断是不是一次完整的响应，
			 * 如果是，根据响应的类型设置响应的状态，并将数据保存到一个全局 buffer
			 */
			if (strstr(buf, "OK\r\n"))          /* 判断字符串中是否含有 OK\r\n，如果有就可以开始解析数据 */
			{
				buf[i+1] = '\0';  // 结尾添加一个结束符，让数据转为合法的字符串
			
				memcpy(g_at_resp, buf, i + 2);      // 将 buf 中长度为 i+2 的数据（包含了结束符）记录到全局数组 g_at_resp
				
				SetATStatus(AT_OK);                    // 设置 AT 响应的状态为 OK
				platform_mutex_unlock(&at_ret_mutex);  // 唤醒 AT 命令发送函数所在的线程
			}
			else if (strstr(buf, "ERROR\r\n"))  /* 判断字符串中是否含有 ERROR\r\n，如果有就可以开始解析数据 */
			{
				buf[i+1] = '\0';  
				
				memcpy(g_at_resp, buf, i + 2);      

				SetATStatus(AT_ERR);  	               // 设置 AT 响应的状态为 ERROR
				platform_mutex_unlock(&at_ret_mutex);  
			}
//			else if (GetSpecialATString(buf))   /* 判断字符串中是否含有特殊的 AT 符，如 "+IPD," */
//			{
//				ProcessSpecialATString(buf);    /* 如果有特殊字符就按特殊情况处理 */ 
//			}
			i++;
		}
	}
}

/* AT 命令的测试任务 */
void Task_ATTest(void *params)
{
	int ret;
	
	while (1)
	{
		ret = ATSendCmd("AT", 2, NULL, 0, 1000);  // 发送命令 "AT"，不保存响应数据，仅测试响应的状态，阻塞 1000 ms
		printf("ATSendCmd ret = %d\r\n", ret);    // 打印响应的状态，OK 或 ERROR 或 TIMEOUT
	}
}

