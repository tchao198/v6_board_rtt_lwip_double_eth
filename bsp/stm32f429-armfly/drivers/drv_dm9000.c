#include <rtthread.h>
#include "board.h"
#include <rtdevice.h>
#include "delay.h"

#ifdef RT_USING_FINSH
#include <finsh.h>
#endif

#define NCR		0x00
#define NSR 	0x01
#define TCR 	0x02
#define TSRI 	0x03
#define TSRII 0x04
#define RCR 	0x05
#define RSR 	0x06
#define ROCR 	0x07

#define BPTR 	0x08
#define FCTR 	0x09
#define FCR 	0x0a
#define EPCR 	0x0b
#define EPAR 	0x0c
#define EPDRL 	0x0d
#define EPDRH 	0x0e

#define WCR 	0x0f
#define PAR1 	0x10
#define PAR2 	0x11
#define PAR3 	0x12
#define PAR4 	0x13
#define PAR5 	0x14
#define PAR6 	0x15

#define MAR1 	0x16
#define MAR2 	0x17
#define MAR3 	0x18
#define MAR4 	0x19
#define MAR5 	0x1a
#define MAR6 	0x1b
#define MAR7 	0x1c
#define MAR8 	0x1d

#define GPCR 	0x1e
#define GPR 	0x1f
#define TRPAL 	0x22
#define TRPAH 	0x23

#define RWPAL 	0x24
#define RWPAH 	0x25

#define CHIPR 	0x2c
#define TCR2	 	0x2d

#define OCR 		0x2e
#define SMCR 		0x2f
#define ETXCSR 	0x30
#define TCSCR 	0x31
#define RCSCSR 	0x32
#define MPAR 		0x33

#define LEDCR 	0x34
#define BUSCR 	0x38
#define INTCR 	0x39
#define SCCR	 	0x50

#define RSCCR 		0x51
#define MRCMDX 		0xf0
#define MRCMDX1 	0xf1
#define MRCMD 		0xf2

#define MRRL 		0xf4
#define MRRH 		0xf5
#define MWCMDX 	0xf6
#define MWCMD 	0xf8
#define MWRL 		0xfa
#define MWRH 		0xfb
#define TXPLL 	0xfc
#define TXPLH 	0xfd
#define ISR 		0xfe
#define IMR 		0xff

#define DM_CMD (*((volatile rt_uint32_t *) 0x64002000))
#define DM_DATA (*((volatile rt_uint32_t *) 0x64002004))
	
#define DM9000_IO_BASE		0x64002000
#define DM9000_DATA_BASE	0x64002004

#define DM9000_IO 			(*((volatile rt_uint16_t *) DM9000_IO_BASE))    // CMD = 0
#define DM9000_DATA 		(*((volatile rt_uint16_t *) DM9000_DATA_BASE))  // CMD = 1

#define DM9000_inb(r) 		(*(volatile rt_uint8_t  *)r)
#define DM9000_outb(r, d) 	(*(volatile rt_uint8_t  *)r = d)
#define DM9000_inw(r) 		(*(volatile rt_uint16_t *)r)
#define DM9000_outw(r, d) 	(*(volatile rt_uint16_t *)r = d)

//#define DEBUG
#ifdef DEBUG
#define STM32_ETH_DM9000_PRINTF          rt_kprintf
#else
#define STM32_ETH_DM9000_PRINTF(...)
#endif

#ifdef RT_USING_LWIP

#include <netif/ethernetif.h>
#include "lwipopts.h"


struct rt_stm32_eth
{
	/* inherit from ethernet device */
	struct eth_device parent;
	
	rt_uint8_t packet_cnt;				  /* packet I or II */
	rt_uint16_t queue_packet_len;		   /* queued packet (packet II) */

	/* interface address info. */
	rt_uint8_t  dev_addr[6];			/* hw address	*/

	uint32_t    ETH_Speed; /*!< @ref ETH_Speed */
	uint32_t    ETH_Mode;  /*!< @ref ETH_Duplex_Mode */
};

static rt_bool_t tx_is_waiting = RT_FALSE;
static struct rt_stm32_eth stm32_eth_device;
static struct rt_semaphore tx_wait,sem_lock;
static uint8_t sendbuffer[2000];
static uint8_t rcvbuffer[2000];

struct eth_hdr
{
	unsigned char d_mac[6];
	unsigned char s_mac[6];
	uint16_t type;
};

struct arp_hdr
{
	struct eth_hdr ethhdr;
	uint16_t hwtype;
	uint16_t protocol;
	unsigned char hwlen;
	unsigned char protolen;
	uint16_t opcode;
	unsigned char smac[6];
	unsigned char sipaddr[4];
	unsigned char dmac[6];
	unsigned char dipaddr[4];
};

struct ip_hdr
{
	struct eth_hdr ethhdr;
	unsigned char vhl,
								tos;
	uint16_t len,
					 ipid,
					 ipoffset;
	unsigned char ttl,
								proto;
	uint16_t ipchksum;
	unsigned char srcipaddr[4],
							 destipaddr[4];
};


uint8_t mac_addr[6] = {0x34,0x44,0x54,0x64,0x74,0x87};

unsigned char Buffer[1000];

uint16_t packet_len;

unsigned char ip_addr[4] = { 192, 168, 2, 138 };

unsigned char host_ip_addr[4] = { 192, 168, 2, 209 };

unsigned char host_mac_addr[6]={ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

#define HON(n) ((((uint16_t)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))


static uint8_t read_phy_reg(uint8_t reg){
	DM_CMD = reg;
	return DM_DATA;
}

static void write_phy_reg(uint8_t reg, uint8_t value){
	DM_CMD = reg;
	DM_DATA = value;
}

void EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    GPIO_Initure.Pin=GPIO_PIN_15;
    GPIO_Initure.Mode=GPIO_MODE_IT_FALLING;
    GPIO_Initure.Pull=GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,0,0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);   
}

unsigned int receivepacket(unsigned char *datas);
void print_buffer(void);

void EXTI15_10_IRQHandler(void)
{
		rt_interrupt_enter();
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
		rt_interrupt_leave();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	
		if(GPIO_Pin == GPIO_PIN_15){
			//rt_kprintf("EXTI:%x\n",GPIO_Pin);
			
			write_phy_reg(IMR, 0x80);
			
			/* Got DM9000 interrupt status */
			int int_status = read_phy_reg(ISR);    /* Got ISR */
			write_phy_reg(ISR, int_status);    /* Clear ISR status */

			/* receive overflow */
			if (int_status & 1<<2)
			{
					rt_kprintf("overflow \r\n");
			}

			if (int_status & 1<<3)
			{
					rt_kprintf("overflow counter overflow \r\n");
			}
				
			if (int_status & 1<<0){
				rt_err_t result;
				result = eth_device_ready(&(stm32_eth_device.parent));
				if( result != RT_EOK )
					rt_kprintf("RX err =%d\n", result );
			}
			
			if (int_status & 1<<1){
				int tx_status = read_phy_reg(NSR);	/* Got TX status */
				if (tx_status & 0x0c)
				{
					stm32_eth_device.packet_cnt --;
					if (stm32_eth_device.packet_cnt > 0)
					{
						rt_kprintf("send second packet\n");
						/* transmit packet II */
						/* Set TX length to DM9000 */
						write_phy_reg(TXPLL, stm32_eth_device.queue_packet_len & 0xff);
						write_phy_reg(TXPLH, (stm32_eth_device.queue_packet_len >> 8) & 0xff);

						/* Issue TX polling command */
						write_phy_reg(TCR, 0x01);	/* Cleared after TX complete */
					}
					rt_sem_release(&tx_wait);
				}
			}
			
			write_phy_reg(IMR, 0x83);
		}
}

void DM9000_init(){
		int i;
	
		
    write_phy_reg(NCR, 0x01);
		delay_us(1000);
    write_phy_reg(NCR, 0x00);
    write_phy_reg(NCR, 0x01);
		delay_us(1000);
    write_phy_reg(NCR, 0x00);
	
		write_phy_reg(GPR, 0x00);
		write_phy_reg(GPCR, 0x01);
    write_phy_reg(GPR, 0x00);
    delay_ms(4);
	
    write_phy_reg(NSR, 0x2c);
    write_phy_reg(ISR, 0x3f);
    write_phy_reg(RCR, 0x39);
    write_phy_reg(TCR, 0x00);
    write_phy_reg(BPTR, 0x3f);
    write_phy_reg(FCTR, 0x3a);
    write_phy_reg(FCR, 0xff);
    write_phy_reg(SMCR, 0x00);

    for(i=0; i<6; i++)
        write_phy_reg(PAR1 + i, mac_addr[i]);

    write_phy_reg(NSR, 0x2c);
    write_phy_reg(ISR, 0x3f);
    write_phy_reg(IMR, 0x83);
		
		EXTI_Init();
}

void sendpacket(unsigned char *datas, unsigned int length)
{
    unsigned int len, i;
		rt_uint16_t last_io;
    last_io = DM9000_IO;
	
    write_phy_reg(IMR, 0x80);
    len = length;
//    write_phy_reg(TXPLH, (len>>8) & 0x0ff);
//    write_phy_reg(TXPLL, len & 0x0ff);

		DM9000_outb(DM9000_IO_BASE, MWCMD);

    for(i=0; i<len;i+=2)
    {
				DM9000_outw(DM9000_DATA_BASE, datas[i] | (datas[i+1]<<8));
    }
		
		if (stm32_eth_device.packet_cnt == 0)
		{
			stm32_eth_device.packet_cnt ++;
			/* Set TX length to DM9000 */
			write_phy_reg(TXPLL, len & 0xff);
			write_phy_reg(TXPLH, (len >> 8) & 0xff);

			/* Issue TX polling command */
			write_phy_reg(TCR, 0x01);	/* Cleared after TX complete */
		}
		else
		{
			stm32_eth_device.packet_cnt ++;
			stm32_eth_device.queue_packet_len = len;
		}
		
    //write_phy_reg(TCR, 0x01);
    //while((read_phy_reg(NSR) & 0x0c) == 0);
		
		write_phy_reg(NSR, 0x2c);
    write_phy_reg(IMR, 0x83);
		
		rt_sem_release(&sem_lock);
		
		rt_sem_take(&tx_wait, RT_WAITING_FOREVER);

		DM9000_IO = last_io;

}

unsigned int receivepacket(unsigned char *datas)
{
    unsigned int i, tem;
    unsigned int status, len, int_status;
    unsigned char ready;
    ready = 0;
    status = 0;
    len = 0;
	
		rt_uint16_t last_io;
    last_io = DM9000_IO;
		
		write_phy_reg(IMR, 0x80);

__error_retry:
		ready = read_phy_reg(MRCMDX);

		if((ready & 0x0ff) != 0x01)
		{
			ready = read_phy_reg(MRCMDX);
			//rt_kprintf("ready:%08x\n",ready);
			if((ready & 0xff) != 0x01)
			{
				if((ready & 0xff) != 0x00){
				 //write_phy_reg(IMR, 0x80);
				 DM9000_init();
				 //write_phy_reg(IMR, 0x83);
				 rt_kprintf("DM9000 reset\n");
				}
				
				write_phy_reg(ISR, 0x3f);
				write_phy_reg(IMR, 0x83);
				return 0;
			}
		}
		
		DM9000_outb(DM9000_IO_BASE, MRCMD);
		status = DM9000_inw(DM9000_DATA_BASE);
		len = DM9000_inw(DM9000_DATA_BASE);

		if(len==0){
			write_phy_reg(IMR, 0x83);
			return 0;
		}
		
    if(!(status & 0xbf00) && (len <= 1536) && (len >63))
    {
        for(i=0; i<len; i+=2)// 16 bit mode
        {
            tem = DM9000_inw(DM9000_DATA_BASE);
            datas[i] = tem & 0xff;
            datas[i + 1] = (tem >> 8) & 0xff;
        }
    }
    else
    { 
			rt_kprintf("\ndddddd len:%u status:%08x ready:%08x\n", len, status, ready);
			for(i=0; i<len; i+=2)// 16 bit mode
			{
					tem = DM9000_inw(DM9000_DATA_BASE);
			}
				
//				goto __error_retry;
			write_phy_reg(IMR, 0x83);
			return 0;
    }
		
		write_phy_reg(IMR, 0x83);
		
		DM9000_IO = last_io;
		return len-4;
}

//unsigned int receivepacket(unsigned char *datas)
//{
//    unsigned int i, tem;
//    unsigned int status, len, int_status;
//    unsigned char ready;
//    ready = 0;
//    status = 0;
//    len = 0;
//	
//		write_phy_reg(IMR, 0x80);

////    if(read_phy_reg(ISR) & 0x01)
////    {
////        write_phy_reg(ISR, 0x01);
////    }
//		
//__error_retry:
//		ready = read_phy_reg(MRCMDX);

//		if((ready & 0x0ff) != 0x01)
//		{
//			ready = read_phy_reg(MRCMDX);
//			//rt_kprintf("ready:%08x\n",ready);
//			if((ready & 0xff) != 0x01)
//			{
//				if((ready & 0xff) != 0x00){
//				 write_phy_reg(IMR, 0x80);
//				 DM9000_init();
//				 write_phy_reg(IMR, 0x83);
//				 rt_kprintf("DM9000 reset\n");
//				}
//				
//				write_phy_reg(IMR, 0x83);
//				return 0;
//			}
//		}

//		
//		status = read_phy_reg(MRCMD);
//    delay_us(200);
//    len = DM_DATA;

//		if(len==0){
//			write_phy_reg(IMR, 0x83);
//			return 0;
//		}
//		
//    if(!(status & 0xbf00) && (len <= 1536) && (len >63))
//    {
//        for(i=0; i<len; i+=2)// 16 bit mode
//        {
//            tem = DM_DATA;
//            datas[i] = tem & 0x0ff;
//            datas[i + 1] = (tem >> 8) & 0x0ff;
//        }
//    }
//    else
//    { 
//			rt_kprintf("\ndddddd len:%u status:%08x ready:%08x\n", len, status, ready);
////				for(i=0; i<len; i+=2)// 16 bit mode
////        {
////            //delay_us(20);
////            tem = DM_DATA;
////        }
//				
////				goto __error_retry;
//			write_phy_reg(IMR, 0x83);
//			return 0;
//    }
//		
//		write_phy_reg(IMR, 0x83);
//		return len;
//}

void arp_request(void)
{
		struct arp_hdr *ARPBUF;
	
		ARPBUF = (struct arp_hdr *)Buffer;
	
		memcpy(ARPBUF->ethhdr.d_mac, host_mac_addr, 6);

		memcpy(ARPBUF->ethhdr.s_mac, mac_addr, 6);

		ARPBUF->ethhdr.type = HON( 0x0806 );

		ARPBUF->hwtype = HON( 1 );

		ARPBUF->protocol = HON( 0x0800 );

		ARPBUF->hwlen = 6;

		ARPBUF->protolen = 4;

		ARPBUF->opcode = HON( 0 );

		memcpy(ARPBUF->smac, mac_addr, 6);

		memcpy(ARPBUF->sipaddr, ip_addr, 4);

		memcpy(ARPBUF->dipaddr, host_ip_addr, 4);

		packet_len = 42;//14+28=42

		sendpacket( Buffer, packet_len );
}

void print_buffer(){
		struct ip_hdr *ETHBUF;
		ETHBUF = (struct ip_hdr *)Buffer;
		char str[100], str1[100], sip[100], dip[100];
		
		if(HON(ETHBUF->ethhdr.type) != 0x806)
			return;
	
		sprintf(str, "d_mac:%02x-%02x-%02x-%02x-%02x-%02x", ETHBUF->ethhdr.d_mac[0],ETHBUF->ethhdr.d_mac[1],ETHBUF->ethhdr.d_mac[2],ETHBUF->ethhdr.d_mac[3],ETHBUF->ethhdr.d_mac[4],ETHBUF->ethhdr.d_mac[5]);
		sprintf(str1, "s_mac:%02x-%02x-%02x-%02x-%02x-%02x", ETHBUF->ethhdr.s_mac[0],ETHBUF->ethhdr.s_mac[1],ETHBUF->ethhdr.s_mac[2],ETHBUF->ethhdr.s_mac[3],ETHBUF->ethhdr.s_mac[4],ETHBUF->ethhdr.s_mac[5]);
		sprintf(sip,"sip:%d.%d.%d.%d", ETHBUF->srcipaddr[0],ETHBUF->srcipaddr[1],ETHBUF->srcipaddr[2],ETHBUF->srcipaddr[3]);
		sprintf(dip,"dip:%d.%d.%d.%d", ETHBUF->destipaddr[0],ETHBUF->destipaddr[1],ETHBUF->destipaddr[2],ETHBUF->destipaddr[3]);
	
		rt_kprintf("%s, %s, %s, %s, type:%d\n", str, str1, sip, dip, HON(ETHBUF->ethhdr.type));
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT_ALIAS(DM9000_init, DM9000_init, );
FINSH_FUNCTION_EXPORT_ALIAS(EXTI_Init, EXTI_Init, );
#endif

/* initialize the interface */
static rt_err_t rt_stm32_eth_init(rt_device_t dev)
{
    STM32_ETH_DM9000_PRINTF("rt_stm32_eth_init...\n");
	
		DM9000_init();

    return RT_EOK;
}

static rt_err_t rt_stm32_eth_open(rt_device_t dev, rt_uint16_t oflag)
{
    STM32_ETH_DM9000_PRINTF("rt_stm32_eth_open...\n");
	return RT_EOK;
}

static rt_err_t rt_stm32_eth_close(rt_device_t dev)
{
    STM32_ETH_DM9000_PRINTF("rt_stm32_eth_close...\n");
	return RT_EOK;
}

static rt_size_t rt_stm32_eth_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    STM32_ETH_DM9000_PRINTF("rt_stm32_eth_read...\n");
	rt_set_errno(-RT_ENOSYS);
	return 0;
}

static rt_size_t rt_stm32_eth_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
    STM32_ETH_DM9000_PRINTF("rt_stm32_eth_write...\n");
	rt_set_errno(-RT_ENOSYS);
	return 0;
}

static rt_err_t rt_stm32_eth_control(rt_device_t dev, int cmd, void *args)
{
    STM32_ETH_DM9000_PRINTF("rt_stm32_eth_control...\n");
	switch(cmd)
	{
	case NIOCTL_GADDR:
		/* get mac address */
		if(args) rt_memcpy(args, stm32_eth_device.dev_addr, 6);
		else return -RT_ERROR;
		break;

	default :
		break;
	}

	return RT_EOK;
}

/* ethernet device interface */
/* transmit packet. */
rt_err_t rt_stm32_eth_dm9000_tx( rt_device_t dev, struct pbuf* p)
{
    rt_err_t ret = RT_ERROR;
    struct pbuf *q;
    uint32_t framelength = 0;
    uint32_t bufferoffset = 0;
    uint32_t byteslefttocopy = 0;
    uint32_t payloadoffset = 0;

    bufferoffset = 0;
    
    STM32_ETH_DM9000_PRINTF("rt_stm32_eth_dm9000_tx...\n");
		//rt_kprintf("rt_stm32_eth_dm9000_tx\n");
    
		rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
    /* copy frame from pbufs to driver buffers */
    for(q = p; q != NULL; q = q->next)
    {
        
        STM32_ETH_DM9000_PRINTF("copy one frame\n");
        
        /* Get bytes in current lwIP buffer */
        byteslefttocopy = q->len;
        payloadoffset = 0;

        /* Check if the length of data to copy is bigger than Tx buffer size*/
        while( (byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE )
        {
            /* Copy data to Tx buffer*/
            memcpy( (uint8_t*)((uint8_t*)sendbuffer + bufferoffset), 
                    (uint8_t*)((uint8_t*)q->payload + payloadoffset), 
                    (ETH_TX_BUF_SIZE - bufferoffset) );


            byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
            payloadoffset = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
            framelength = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
            bufferoffset = 0;
        }

        /* Copy the remaining bytes */
        memcpy( (uint8_t*)((uint8_t*)sendbuffer + bufferoffset), 
                (uint8_t*)((uint8_t*)q->payload + payloadoffset), 
                byteslefttocopy );
        bufferoffset = bufferoffset + byteslefttocopy;
        framelength = framelength + byteslefttocopy;
    }
    

    /* Prepare transmit descriptors to give to DMA */ 
    STM32_ETH_DM9000_PRINTF("dm9000 transmit frame, length: %d\n", framelength);
		//rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
    sendpacket(sendbuffer, framelength);
		//rt_sem_release(&sem_lock);

    ret = ERR_OK;
        
    return ret;
}

/* reception packet. */
struct pbuf *rt_stm32_eth_dm9000_rx(rt_device_t dev)
{
    struct pbuf *p = NULL;
    struct pbuf *q = NULL;
    uint16_t len = 0;
    uint32_t bufferoffset = 0;
    uint32_t payloadoffset = 0;
    uint32_t byteslefttocopy = 0;
	
    STM32_ETH_DM9000_PRINTF("rt_stm32_eth_dm9000_rx\n");

		rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
		len = receivepacket(rcvbuffer);
		//rt_sem_release(&sem_lock);

		//rt_kprintf("recive len:%d\n", len);
		STM32_ETH_DM9000_PRINTF("dm9000 receive frame len : %d\n", len);

    if (len > 0)
    {
        /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    }
		else{

		}
    
    if (p != NULL)
    {
        bufferoffset = 0;
        for(q = p; q != NULL; q = q->next)
        {
            byteslefttocopy = q->len;
            payloadoffset = 0;

            /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size*/
            while( (byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE )
            {
								rt_kprintf("> ETH_RX_BUF_SIZE\n");
                /* Copy data to pbuf */
                memcpy( (uint8_t*)((uint8_t*)q->payload + payloadoffset), (uint8_t*)((uint8_t*)rcvbuffer + bufferoffset), (ETH_RX_BUF_SIZE - bufferoffset));

                byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
                payloadoffset = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
                bufferoffset = 0;
            }
            /* Copy remaining data in pbuf */
            memcpy( (uint8_t*)((uint8_t*)q->payload + payloadoffset), (uint8_t*)((uint8_t*)rcvbuffer + bufferoffset), byteslefttocopy);
            bufferoffset = bufferoffset + byteslefttocopy;
        }
    }
		
		rt_sem_release(&sem_lock);

	return p;
}


static int rt_hw_stm32_dm9000_init(void)
{
    rt_err_t state;
    
    stm32_eth_device.ETH_Speed = ETH_SPEED_100M;
    stm32_eth_device.ETH_Mode  = ETH_MODE_FULLDUPLEX;
	
		stm32_eth_device.packet_cnt = 0;
		stm32_eth_device.queue_packet_len = 0;

    /* OUI 00-80-E1 STMICROELECTRONICS. */
    stm32_eth_device.dev_addr[0] = 0x11;
    stm32_eth_device.dev_addr[1] = 0x80;
    stm32_eth_device.dev_addr[2] = 0xE1;
    /* generate MAC addr from 96bit unique ID (only for test). */
    stm32_eth_device.dev_addr[3] = *(rt_uint8_t*)(UID_BASE+4);
    stm32_eth_device.dev_addr[4] = *(rt_uint8_t*)(UID_BASE+2);
    stm32_eth_device.dev_addr[5] = *(rt_uint8_t*)(UID_BASE+0);

    stm32_eth_device.parent.parent.init       = rt_stm32_eth_init;
    stm32_eth_device.parent.parent.open       = rt_stm32_eth_open;
    stm32_eth_device.parent.parent.close      = rt_stm32_eth_close;
    stm32_eth_device.parent.parent.read       = rt_stm32_eth_read;
    stm32_eth_device.parent.parent.write      = rt_stm32_eth_write;
    stm32_eth_device.parent.parent.control    = rt_stm32_eth_control;
    stm32_eth_device.parent.parent.user_data  = RT_NULL;

    stm32_eth_device.parent.eth_rx     = rt_stm32_eth_dm9000_rx;
    stm32_eth_device.parent.eth_tx     = rt_stm32_eth_dm9000_tx;

    STM32_ETH_DM9000_PRINTF("sem init: tx_wait\r\n");
    /* init tx semaphore */
    rt_sem_init(&tx_wait, "tx_wait", 1, RT_IPC_FLAG_FIFO);
		rt_sem_init(&sem_lock, "eth_lock", 1, RT_IPC_FLAG_FIFO);

    /* register eth device */
    STM32_ETH_DM9000_PRINTF("eth_device_init start\r\n");
    state = eth_device_init(&(stm32_eth_device.parent), "e1");
    if (RT_EOK == state)
    {
        STM32_ETH_DM9000_PRINTF("eth_device_init success\r\n");
    }
    else
    {
        STM32_ETH_DM9000_PRINTF("eth_device_init faild: %d\r\n", state);
    }
    return state;
}
INIT_DEVICE_EXPORT(rt_hw_stm32_dm9000_init);


void dm9000a(void)
{
	rt_kprintf("\n");
	rt_kprintf("NCR   (%02X): %02x\n", NCR,   read_phy_reg(NCR));
	rt_kprintf("NSR   (%02X): %02x\n", NSR,   read_phy_reg(NSR));
	rt_kprintf("TCR   (%02X): %02x\n", TCR,   read_phy_reg(TCR));
	rt_kprintf("TSRI  (%02X): %02x\n", TSRI,  read_phy_reg(TSRI));
	rt_kprintf("TSRII (%02X): %02x\n", TSRII,  read_phy_reg(TSRII));
	rt_kprintf("RCR   (%02X): %02x\n", RCR,   read_phy_reg(RCR));
	rt_kprintf("RSR   (%02X): %02x\n", RSR,   read_phy_reg(RSR));
	rt_kprintf("ORCR  (%02X): %02x\n", ROCR,  read_phy_reg(ROCR));
	rt_kprintf("CRR   (%02X): %02x\n", CHIPR, read_phy_reg(CHIPR));
	rt_kprintf("CSCR  (%02X): %02x\n", TCSCR,  read_phy_reg(TCSCR));
	rt_kprintf("RCSSR (%02X): %02x\n", RCSCSR, read_phy_reg(RCSCSR));
	rt_kprintf("ISR   (%02X): %02x\n", ISR,   read_phy_reg(ISR));
	rt_kprintf("IMR   (%02X): %02x\n", IMR,   read_phy_reg(IMR));
	rt_kprintf("\n");
}
#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(dm9000a, dm9000a register dump);
#endif

#endif
