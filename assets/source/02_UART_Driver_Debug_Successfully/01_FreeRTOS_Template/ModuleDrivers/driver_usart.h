/*  Copyright (s) 2019 ���ڰ������Ƽ����޹�˾
 *  All rights reserved
 * 
 * �ļ����ƣ�driver_usart.h
 * ժҪ��
 *  
 * �޸���ʷ     �汾��        Author       �޸�����
 *--------------------------------------------------
 * 2020.6.6      v01        ���ʿƼ�      �����ļ�
 *--------------------------------------------------
*/

#ifndef __DRIVER_USART_H
#define __DRIVER_USART_H

#include "stm32f1xx_hal.h"

/*
 *  ��������void EnableDebugIRQ(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã�ʹ��USART1���ж�
*/
extern void EnableDebugIRQ(void);

/* ������  �� EnableUART2IRQ
 * ��������� ��
 * ��������� ��
 * ����ֵ  �� ��
 * �������ܣ� ʹ�� USART2 �Ľ����ж�
*/
extern void EnableUART2IRQ(void);
/*
 *  ��������void DisableDebugIRQ(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã�ʧ��USART1���ж�
*/
extern void DisableDebugIRQ(void);

#endif /* __DRIVER_USART_H */
