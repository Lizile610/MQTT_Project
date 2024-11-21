/* �ļ����ƣ�ring_buffer.h
 * ժҪ��
 *  
 * �޸���ʷ      �汾��      Author      �޸�����
 *--------------------------------------------------
 * 2024.11.3      v01        Lizile      �����ļ�
 *--------------------------------------------------
*/

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include "stm32f1xx_hal.h"   /* HAL����� */

#define BUFFER_SIZE  1024    /* ���λ������Ĵ�С */

/* ���λ������ṹ�� */
typedef struct
{
	unsigned char buffer[BUFFER_SIZE];  /* ���λ������ռ� */
	volatile unsigned int pW;           /* дλ�� */
	volatile unsigned int pR;           /* ��λ�� */
} ring_buffer;

/* ������  �� ring_buffer_init
 * ��������� dst_buf --> ָ��Ŀ�껷�λ�����
 * ��������� ��
 * ����ֵ  �� ��
 * �������ܣ� ��ʼ��������
*/
extern void ring_buffer_init(ring_buffer *dst_buf);

/* ������  �� ring_buffer_write
 * ��������� dst_buf --> ָ��Ŀ�껷�λ�����
 *            c --> Ҫд�������
 * ��������� ��
 * ����ֵ  �� ��
 * �������ܣ� ��Ŀ�껷�λ�����д��һ���ֽڵ����ݣ������������������������
*/
extern void ring_buffer_write(unsigned char c, ring_buffer *dst_buf);

/* ������  �� ring_buffer_read
 * ��������� dst_buf --> ָ��Ŀ�껷�λ�����
 *            c --> ��Ŀ�껷�λ��������������ݱ��浽�ڴ����ĸ���ַ
 * ��������� ��
 * ����ֵ  �� ��ȡ�����ݣ����� 0�����򣬷��� -1
 * �������ܣ� ��Ŀ�껷�λ�������ȡһ���ֽڵ����ݱ��浽�ڴ��У�    \
              ���Ŀ�껷�λ������ѿգ����� -1����ʾ��ȡʧ��
*/
extern int ring_buffer_read(unsigned char *c, ring_buffer *dst_buf);

#endif /* __RING_BUFFER_H */

����һ��ʾ��    ������һ��ʾ��







