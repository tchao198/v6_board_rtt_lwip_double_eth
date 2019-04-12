#ifndef __UDP_DEMO_H
#define __UDP_DEMO_H
#include <rtthread.h>
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F4&F7������
//NETCONN API��̷�ʽ��UDP���Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/2/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
typedef rt_uint8_t u8;
typedef rt_uint16_t u16;
typedef rt_uint32_t u32;
 
#define UDP_DEMO_RX_BUFSIZE		2000	//����udp���������ݳ���
#define UDP_DEMO_PORT			8089	//����udp���ӵı��ض˿ں�
#define LWIP_SEND_DATA			0X80    //���������ݷ���

extern u8 udp_flag;		//UDP���ݷ��ͱ�־λ
void udp_demo_test(void);
#endif

