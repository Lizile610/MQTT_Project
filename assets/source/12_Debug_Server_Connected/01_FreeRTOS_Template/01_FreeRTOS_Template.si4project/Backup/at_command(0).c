/* 文件名称：at_command.c
 * 摘要    ：AT 命令相关函数
 * 修改历史：2024.11.8  
 * 修改内容：增加网络数据的收包函数
 *----------------------------------------
 * Version ：v01 	  
 * Author  ：Lizile	   
 */

#include "platform_mutex.h"  /* 平台互斥量相关函数 */
#include "at_command.h"	
#include "at_uart_hal.h"     /* AT 命令的 UART 硬件接口程序 */

#define AT_CMD_TIMEOUT 1000  /* 等待 AT 命令响应的超时时间 */
#define AT_RESP_LEN    100   /* g_at_resp 的 buffer 大小，一次 AT 命令响应中超出此长度的数据将不被保存 */
#define AT_PACK_LEN    100   /* g_at_packet 的 buffer 大小，一次网络数据中超出此长度的数据将不被保存 */

static platform_mutex_t at_resp_mutex;     // 等待 AT 命令响应的互斥量
static platform_mutex_t at_packet_mutex;   // 等待网络数据包的互斥量

static int g_at_status;  			       // AT 命令响应的状态
static char g_at_resp[AT_RESP_LEN];        // 全局buffer，保存解析线程中传递过来的 AT 命令响应
static char g_at_packet[AT_PACK_LEN];      // 全局buffer，保存解析线程中传递过来的 “不请自来” 的网络数据
static int g_at_packet_len;                // 网络数据的长度信息

/* 函数名  ： SetATStatus
 * 输入参数： status --> AT 命令响应的状态
 *              AT_OK、AT_ERR、AT_TIMEOUT 
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 设置 AT 命令响应的状态，OK 还是 ERROR 还是超时？
 */
void SetATStatus(int status)
{
	g_at_status = status;
}

/* 函数名  ： GetATStatus
 * 输入参数： 无
 * 输出参数： 无
 * 返回值  ： 
 * 函数功能： 获得 AT 命令响应的状态，OK 还是 ERROR 还是超时？
 */
int GetATStatus(void)
{
	return g_at_status;
}

/* 函数名  ： AT_Init
 * 输入参数： 无
 * 输出参数： 无
 * 返回值  ： 
 * 函数功能： 初始化 AT 命令相关内容，如互斥量
 */
int AT_Init(void)
{
	platform_mutex_init(&at_resp_mutex);    // 创建等待 AT 命令响应的互斥量，初始为 1
	platform_mutex_lock(&at_resp_mutex);    // 先上锁(让mutex=0)，只有当解析线程解锁(mutex=1)，才能唤醒发送线程

	
	platform_mutex_init(&at_packet_mutex);  // 创建等待网络数据的互斥量，初始为 1
	platform_mutex_lock(&at_packet_mutex);  // 先上锁(让mutex=0)，只有当解析线程解锁(mutex=1)，才能唤醒等待网络数据的线程
}

/* 函数名  ： ATSendCmd
 * 输入参数： buf --> AT 命令
 *            len --> AT 命令的数据长度
 *			  resp --> 保存 AT 命令的响应信息
 *            resp_len --> 希望得到多长的返回信息
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
	
	/* 等待 AT 命令的响应
	 * ret : 
	 *      1 - 成功得到互斥量，AT 命令的响应状态可能为 AT_OK 或 AT_ERR
	 * 	    0 - 超时返回，AT 命令的响应状态为 AT_TIMEOUT
	 */
	ret = platform_mutex_lock_timeout(&at_resp_mutex, AT_CMD_TIMEOUT);  // 获得信号量，并且让信号量 -1

	/* 判断 AT 命令的响应 */
	if (ret)
	{
		/* 如果有响应，将 AT 命令的响应信息存入 resp */
		err = GetATStatus();  // 获得 AT 命令响应的状态
		
		if (!err && resp)
		{
			/* 如果 AT 命令返回 OK，则将缓冲区 g_at_resp 中解析好的数据存入 resp（期望的数据长度为 resp_len）
		     * 注意不要越界：g_at_resp 中最多只有 AT_RESP_LEN 个数据
		     */
			memcpy(resp, g_at_resp, resp_len > AT_RESP_LEN ? AT_RESP_LEN : resp_len);  
		}
		return err;         // 返回 AT 命令响应的状态，可能为 AT_OK 或 AT_ERR
	}
	else
	{
		return AT_TIMEOUT;  // 超时返回
	}
}

/* 函数名  ： GetSpecialATString
 * 输入参数： buf -> 暂存 ESP8266 返回信息的缓冲区
 * 输出参数： 无
 * 返回值  ： 1 - 在 ESP8266 的返回信息中找到了特殊字符串
 * 	          0 - 未找到特殊字符串
 * 函数功能： 在 ESP8266 的返回信息中查找是否存在表示网络数据包的特殊字符串
 */
int GetSpecialATString(char *buf)
{
	if (strstr(buf, "+IPD,"))  /* 在 AT 的响应信息中查找 "+IPD," */
		return 1;
	else
		return 0;
}

/* 函数名  ： ProcessSpecialATString
 * 输入参数： buf -> 暂存 ESP8266 返回信息的缓冲区
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 处理 ESP8266 返回信息中的网络数据包
 */
void ProcessSpecialATString(char *buf)
{
	int i = 0;
	int len = 0;
	
	/* 假设网络数据包的完整格式为： +IPD,78:xxxxxx
	 * 其中 78 表示有效数据 xxxxxx 的长度
	 */
	while (1)
	{
		/* 解析有效数据 xxxxxx 的长度
		 * 将 ':' 前的字符串转换为整型，就是有效数据长度
		 */
		i = 0;
		while (1)
		{
			HAL_AT_Recv(&buf[i], portMAX_DELAY);  // 让 buf 重新从 0 位置接收 ',' 后面的数据
			if (buf[i] == ':')
				break;  // 如果接收到了 ':'，就表示前几次循环已经将长度信息解析完毕，退出循环
			else
			{
				len = len * 10 + (buf[i] - '0');  // 将接收到数字字符串转换为长度信息（字符串型 -> 整型）
			}
			i++;
		}

		/* 读取 "+IPD,78:" 后面真正的数据，存入专用的缓冲区 */
		i = 0;
		while (i < len)
		{
			HAL_AT_Recv(&buf[i], portMAX_DELAY);  // 让 buf 重新从 0 位置接收 ':' 后面真正的数据
			
			/* 将这一次 “不请自来” 的网络数据转存到专用的全局缓冲区，超出缓冲区容量的数据会被丢弃 */
			if (i < AT_PACK_LEN)
			{
				g_at_packet[i] = buf[i];
				g_at_packet_len = i;      // 记录网络数据的长度，最大为 AT_PACK_LEN
			}
			
			i++;
		}
	}
}

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
int ATReadPacket(char *buf, int len, int *read_len, int timeout)
{
	int ret;
	
	ret = platform_mutex_lock_timeout(&at_packet_mutex, timeout);  /* 获得互斥锁，被通知有网络数据包可读 */

	if (ret)
	{
		/* 如果有网络数据包可读，则将网络数据存到 buf，记录实际读取到的数据长度 */
		*read_len = len > g_at_packet_len ? g_at_packet_len : len;
		memcpy(buf, g_at_packet, *read_len);
		return AT_OK;
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
	char buf[AT_RESP_LEN];  // 从 ESP8266 模块读到的数据会暂存在这个 buf
	int i = 0;

	while (1)
	{
		/* 逐个字节读取从 ESP8266 模块返回来的数据，暂存到 buf 中
		 * 使用阻塞的方式，将阻塞时间设置为最大，如果 ESP8266 一直没有返回来数据，就一直等待
		 */
		HAL_AT_Recv(&buf[i], portMAX_DELAY);

		/* 解析数据：
		 * 每当从 ESP8266 返回来的数据中读到 "\r\n" 时判断是否返回了一次完整的响应，如果是就转存响应信息；
		 * 另外，如果读到了特殊的网络数据包，则解析并转存
		 */
		if (i && (buf[i-1] == '\r') && (buf[i] == '\n'))
		{
			/* 如果碰到了回车换行，根据以下三种情况判断是不是一次完整的响应，
			 * 如果是，根据响应的类型设置响应的状态，并将数据保存到一个全局 buffer
			 */
			if (strstr(buf, "OK\r\n"))          /* 判断字符串中是否含有 OK\r\n，如果有就可以开始解析数据 */
			{
				buf[i+1] = '\0';                // 结尾添加一个结束符，让数据转为合法的字符串
			
				memcpy(g_at_resp, buf, i + 2);  // 将 buf 中长度为 i+2 的数据（包含了结束符）转存到全局缓冲区 g_at_resp
				
				SetATStatus(AT_OK);                     // 设置 AT 响应的状态为 OK
				platform_mutex_unlock(&at_resp_mutex);  // 唤醒 AT 命令发送函数所在的线程

				i = 0;  // 已经读取到一次完整的响应，重置下一次保存
			}
			else if (strstr(buf, "ERROR\r\n"))  /* 判断字符串中是否含有 ERROR\r\n，如果有就可以开始解析数据 */
			{
				buf[i+1] = '\0';  
				
				memcpy(g_at_resp, buf, i + 2);      

				SetATStatus(AT_ERR);  	                // 设置 AT 响应的状态为 ERROR
				platform_mutex_unlock(&at_resp_mutex);  

				i = 0;  
			}
			else
			{
				/* 如果只接收到回车换行，说明还不是一次完整的响应，继续读 */
				i++;
			}
		}
		else if (GetSpecialATString(buf))	/* 判断字符串中是否含有特殊的字符串，如 "+IPD," */
		{
			ProcessSpecialATString(buf);			  /* 如果有特殊字符就按特殊情况处理 */ 
			platform_mutex_unlock(&at_packet_mutex);  /* 释放互斥锁，唤醒等待网络数据包的线程 */

			i = 0;
		}
		else
			i++;
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

