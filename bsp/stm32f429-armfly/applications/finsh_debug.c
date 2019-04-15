#include <rtthread.h>
#include <dfs_posix.h>
#include <string.h>

#include "delay.h"

#define DM_CMD (*((volatile unsigned int *) 0x64002000))
#define DM_DATA (*((volatile unsigned int *) 0x64002004))

void sub_version()
{
	rt_kprintf("\n \\ | /\n");
	rt_kprintf("- RT -     Apollo Uart Project\n");
	rt_kprintf(" / | \\     %d.%d.%d build %s\n",
						 RT_VERSION, RT_SUBVERSION, RT_REVISION, __DATE__);
}

void readDM9000Reg(uint8_t reg){
	uint32_t data;
	DM_CMD	= reg;
	delay_us(20);
	data = DM_DATA;
	delay_us(20);
	rt_kprintf("data:%08xH", data);
}

void writeDM9000Reg(uint8_t reg, uint32_t value){
	DM_CMD = reg;
	delay_us(20);
	DM_DATA = value;
	delay_us(20);
}

uint32_t readDM9000ID(){
	uint32_t data=0;
	DM_CMD	= 0x28;
	delay_us(20);
	data = DM_DATA;
	delay_us(20);
	DM_CMD	= 0x29;
	delay_us(20);
	data += DM_DATA<<8;
	delay_us(20);
	DM_CMD	= 0x2a;
	delay_us(20);
	data += DM_DATA<<16;
	delay_us(20);
	DM_CMD	= 0x2b;
	delay_us(20);
	data += DM_DATA<<24;
	delay_us(20);
	rt_kprintf("DM9000AE ID:%08x\n", data);
	
	return data;
}

#define LED_ON(n) (*((volatile uint32_t *) 0x64001000)) = 0xffff & ~(1<<(n+8));  for(int j=0;j<10;j++){;}  //必须等待一段时间使得锁存器电平改变才能点亮LED
#define LED_OFF(n) (*((volatile uint32_t *) 0x64001000)) = 0xffff | (1<<(n+8));  for(int j=0;j<10;j++){;}
void led(int num){
	for(int i=0;i<5;i++){
		LED_ON(num);
		rt_thread_delay(1000);
		LED_OFF(num);
		rt_thread_delay(1000);
	}
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT_ALIAS(readDM9000Reg, readDM, readDM(reg));
FINSH_FUNCTION_EXPORT_ALIAS(writeDM9000Reg, writeDM, writeDM(reg, value));
FINSH_FUNCTION_EXPORT_ALIAS(readDM9000ID, __cmd_readDMID,);
FINSH_FUNCTION_EXPORT_ALIAS(led, led,);
#endif
