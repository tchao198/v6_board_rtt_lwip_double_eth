#include "udp_demo.h"

#include "usart.h"
#include "lwip/api.h"
#include "lwip/lwip_sys.h"
//#include "lwip/ip4.h"
#include "lwip/ip_addr.h"
#include "string.h"
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
 
//TCP�ͻ�������
#define UDP_PRIO		6
//�����ջ��С
#define UDP_STK_SIZE	300

//extern struct rt_mailbox mb;
//extern rt_sem_t sem;
//extern volatile uint32_t CT_flag;


u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE];	//UDP�������ݻ�����
//UDP������������
const u8 *udp_demo_sendbuf="Apollo STM32F4/F7 NETCONN UDP demo send data\r\n";
u8 udp_flag = 0x80;							//UDP���ݷ��ͱ�־λ
u32 data_len = 0;
struct pbuf *q;

//udp������
void udp_demo_test(void)
{
	err_t err;
	static struct netconn *udpconn;
	static struct netbuf  *recvbuf;
	static struct netbuf  *sentbuf;
	ip_addr_t destipaddr, localipaddr;
	
	udpconn = netconn_new(NETCONN_UDP);  //����һ��UDP����
	udpconn->recv_timeout = 10; 
	
	if(udpconn != NULL)  //����UDP���ӳɹ�
	{
		IP4_ADDR(&localipaddr, 192, 168, 2, 250);
		err = netconn_bind(udpconn,&localipaddr,UDP_DEMO_PORT); 
		
		IP4_ADDR(&destipaddr, 192, 168, 2, 209); //����Ŀ��IP��ַ
    netconn_connect(udpconn,&destipaddr,UDP_DEMO_PORT); 	//���ӵ�Զ������
		if(err == ERR_OK)//�����
		{
			while(1)
			{
				if((udp_flag & LWIP_SEND_DATA) == LWIP_SEND_DATA) //������Ҫ����
				{
					//rt_kprintf("udp_send_data\n");
					
					sentbuf = netbuf_new();
					netbuf_alloc(sentbuf,strlen((char *)udp_demo_sendbuf));
					memcpy(sentbuf->p->payload,(void*)udp_demo_sendbuf,strlen((char*)udp_demo_sendbuf));
					
					err = netconn_send(udpconn,sentbuf);  	//��netbuf�е����ݷ��ͳ�ȥ
					if(err != ERR_OK)
					{
						rt_kprintf("����ʧ��:%d\r\n", err);
					}
					//udp_flag &= ~LWIP_SEND_DATA;	//������ݷ��ͱ�־
					netbuf_delete(sentbuf);      	//ɾ��buf
				}
				rt_thread_delay(1);  //��ʱ5ms
				
				netconn_recv(udpconn,&recvbuf); //��������
				if(recvbuf != NULL)          //���յ�����
				{ 
					//rt_kprintf("recive ok\n");
					//OS_ENTER_CRITICAL(); //���ж�
					memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //���ݽ��ջ���������
					for(q=recvbuf->p;q!=NULL;q=q->next)  //����������pbuf����
					{
						//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
						//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
						if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//��������
						else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
						data_len += q->len;  	
						if(data_len > UDP_DEMO_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
					}
					//OS_EXIT_CRITICAL();  //���ж�
					data_len=0;  //������ɺ�data_lenҪ���㡣
					rt_kprintf("%s\r\n",udp_demo_recvbuf);  //��ӡ���յ�������
					netbuf_delete(recvbuf);      //ɾ��buf
					//udp_flag |= 0x80;
				}else rt_thread_delay(5);  //��ʱ5ms
			}
			
			//netbuf_delete(sentbuf);      	//ɾ��buf
			
		}else rt_kprintf("UDP��ʧ��\r\n");
	}else rt_kprintf("UDP���Ӵ���ʧ��\r\n");
}




