/* 文件名称：platform_net_socket.c
 * 摘要    ：
 * 修改历史：2024.11.10  
 * 修改内容：实现网络连接
 *----------------------------------------
 * Version ：v01 	  
 * Author  ：Lizile	   
 */
 
#include "mqtt_log.h"           
#include "platform_net_socket.h"
#include "at_command.h"
#include <stdio.h>

#define TEST_SSID      "Tenda_A811A0"  /* 路由器 ID  */
#define TEST_PASSWORD  "15219642833"   /* 路由器密码 */

/* 函数名  ： platform_net_socket_connect
 * 输入参数： host --> 服务器 IP 地址
 *            port --> 服务器端口号
 *            proto --> 协议，TCP 或 UDP
 * 输出参数： 无
 * 返回值  ： < 0, err
 *            = 0, ok
 * 函数功能： 将 WiFi 模块连接到服务器
 *------------------------------------------------------------
 * 修改时间： 2024.11.13
 * 修改内容： 重连 WiFi 前增加断开网络连接
 */
int platform_net_socket_connect(const char *host, const char *port, int proto)
{
	int err;
	char cmd[100];  /* 构造的网络连接命令 */

	/* 1. 先断开原来的 WiFi 连接 */
	err = ATSendCmd("AT+CWQAP", NULL, 0, 2000);
	if (err)
	{
		printf("Disconnect AP err = %d\n", err);  // 打印错误信息
		return err;
	}	
	
	/* 2. 配置 WiFi 模式 */
	err = ATSendCmd("AT+CWMODE=3", NULL, 0, 2000);
	if (err)
	{
		printf("Send \"AT+CWMODE=3\" err = %d\n", err);  // 打印错误信息
		return err;
	}	
	
	/* 3. 连接路由器 */
	err = ATSendCmd("AT+CWJAP=\"" TEST_SSID "\",\"" TEST_PASSWORD "\"", NULL, 0, 20000);
	if (err)
	{
		printf("Connect AP err = %d\n", err);  // 打印错误信息
		return err;
	}	

	/* 4. 连接服务器
	 *    根据传入的 proto 取值构造网络连接命令，建立 TCP 连接或 UDP 连接
	 */
	if (proto == PLATFORM_NET_PROTO_TCP)
	{
		/* AT+CIPSTART="TCP","192.168.0.202",1883 */
		sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", host, port);  /* 构造 TCP 连接命令 */
	}
	else if (proto == PLATFORM_NET_PROTO_UDP)
	{
		sprintf(cmd, "AT+CIPSTART=\"UDP\",\"%s\",%s", host, port);  /* 构造 TCP 连接命令 */
	}

	err = ATSendCmd(cmd, NULL, 0, 5000);
	if (err)
	{
		printf("%s err = %d\n", cmd, err);  // 打印错误信息
		return err;
	}	

	return 0;
}

#if 0
int platform_net_socket_recv(int fd, void *buf, size_t len, int flags)  /* 读，接收数据 */
{
	return 0;
}
#endif

/* 函数名  ： platform_net_socket_recv_timeout
 * 输入参数： buf --> 收到的网络数据放入该缓冲区
 *            len --> 收到的数据长度
 *            timeout --> 超时时间
 * 输出参数： 无
 * 返回值  ： 实际收到的数据长度
 * 函数功能： 网络收包函数
 *------------------------------------------------------------
 * 修改时间： 2024.11.11
 * 修改内容： 创建函数
 */
int platform_net_socket_recv_timeout(int fd, unsigned char *buf, int len, int timeout)  
{
	int i = 0;
	int err;
	
	/* 读数据，读不到就阻塞 */
	while (i < len)
	{
		err = ATReadData(&buf[i], timeout);  // 每次调用都读一个字符
		if (err)

		{
			printf("ATReadData err = %d\n", err);  // 打印错误信息
			return 0;  // 如果发生错误，返回 0，表示读不到数据
		}
		
		i++;
	}

	return len;
}

#if 0
int platform_net_socket_write(int fd, void *buf, size_t len)  /* 写，发送数据 */
{
	return 0;
}
#endif

/* 函数名  ： platform_net_socket_write_timeout
 * 输入参数： buf --> 要向服务器发送的数据
 *            len --> 数据长度
 *            timeout --> 超时时间，单位为 tick
 * 输出参数： 无
 * 返回值  ： 本次发送数据得到的响应状态
 * 函数功能： 向服务器发送网络数据
 *--------------------------------------------------------------------
 * 修改时间： 2024.11.11
 * 修改内容： 创建函数
 */
int platform_net_socket_write_timeout(int fd, unsigned char *buf, int len, int timeout)  
{
	int err;
	char cmd[20];

	/* 根据将要发送的数据长度，构造命令 */
	sprintf(cmd, "AT+CIPSEND=%d", len);	

	/* 发送命令，表示准备要发送网络数据 */
	err = ATSendCmd(cmd, NULL, 0, timeout);
	if (err)
	{
		printf("%s err = %d\n", cmd, err);  // 打印错误信息
		return err;
	}	

	/* 开始发送网络数据 */
	err = ATSendData(buf, len, timeout);

	if (err)
	{
		printf("ATSendData err = %d\n", err);	// 打印错误信息
		return err;
	}	

	return len;
}

/* 函数名  ： platform_net_socket_close
 * 输入参数： fd
 * 输出参数： 无
 * 返回值  ： < 0, err
 *            = 0, ok
 * 函数功能： 断开 WiFi 模块的网络连接
 *------------------------------------------------------------
 * 修改时间： 2024.11.10
 * 修改内容： 创建函数
 */
int platform_net_socket_close(int fd)
{
	return ATSendCmd("AT+CIPCLOSE", NULL, 0, 2000);
}

#if 0
int platform_net_socket_set_block(int fd)
{
	return 0;
}

int platform_net_socket_set_nonblock(int fd)
{
	return 0;
}

int platform_net_socket_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
	return 0;
}
#endif

