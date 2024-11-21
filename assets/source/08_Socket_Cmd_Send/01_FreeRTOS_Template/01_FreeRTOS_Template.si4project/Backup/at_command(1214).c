
#include "platform_mutex.h"  /* 平台互斥量相关函数 */
#include "at_command.h"		 	

#define AT_CMD_TIMEOUT 1000  /* 等待 AT 命令返回结果的超时时间 */
#define AT_RESP_LEN    100   /* g_at_resp 的 buffer 大小 */

static platform_mutex_t at_ret_mutex;  // 等待 AT 命令返回结果的信号量
static int g_at_status;  			   // AT 命令返回结果的状态
static char g_at_resp[AT_RESP_LEN];    // 全局buffer，保存解析线程中解析好的数据

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
	ret = platform_mutex_lock(&at_ret_mutex, AT_CMD_TIMEOUT);  // 获得信号量，并且让信号量 -1

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

