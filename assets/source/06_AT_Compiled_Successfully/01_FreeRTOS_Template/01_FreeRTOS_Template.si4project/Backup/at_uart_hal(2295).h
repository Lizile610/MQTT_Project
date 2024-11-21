
/* 函数名  ： HAL_AT_Send
 * 输入参数： buf --> 要发送给 ESP8266 的 AT 命令或数据
 *            len --> 要发送的数据长度
 * 输出参数： 无
 * 返回值  ： 无
 * 函数功能： 选择不同的硬件接口向 ESP8266 发送 AT 命令，目前只有 UART2 接口
*/
void HAL_AT_Send(char *buf, int len)
{
	USART2_Write(buf, len);
}
