#include "drv_dac.h"
#include "rtthread.h"
//////////////////////////////////////////////////////////////////////////////////	 
//�ڲ�DAC����				  
////////////////////////////////////////////////////////////////////////////////// 	

DAC_HandleTypeDef DAC_Handler;//DAC���

//��ʼ��DAC
int DAC_Init(void)
{
    DAC_ChannelConfTypeDef DAC_Config;
    
    DAC_Handler.Instance=DAC;
    HAL_DAC_Init(&DAC_Handler);                 //��ʼ��DAC
    
    DAC_Config.DAC_Trigger=DAC_TRIGGER_NONE;             //��ʹ�ô�������
    DAC_Config.DAC_OutputBuffer=DAC_OUTPUTBUFFER_DISABLE;//DAC1�������ر�
    HAL_DAC_ConfigChannel(&DAC_Handler,&DAC_Config,DAC_CHANNEL_1);//DACͨ��1����
		HAL_DAC_ConfigChannel(&DAC_Handler,&DAC_Config,DAC_CHANNEL_2);//DACͨ��1����
    
    HAL_DAC_Start(&DAC_Handler,DAC_CHANNEL_1);  //����DACͨ��1
		HAL_DAC_Start(&DAC_Handler,DAC_CHANNEL_2);  //����DACͨ��1
	
		return 0;
}
INIT_DEVICE_EXPORT(DAC_Init);

//DAC�ײ�������ʱ�����ã����� ����
//�˺����ᱻHAL_DAC_Init()����
//hdac:DAC���
void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac)
{      
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_DAC_CLK_ENABLE();             //ʹ��DACʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_4 | GPIO_PIN_5;            //PA4
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //ģ��
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

//����ͨ��1�����ѹ
//vol:0~3300,����0~3.3V
void DAC1_Set_Vol(u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
    HAL_DAC_SetValue(&DAC_Handler,DAC_CHANNEL_1,DAC_ALIGN_12B_R,temp);//12λ�Ҷ������ݸ�ʽ����DACֵ
}

//����ͨ��2�����ѹ
//vol:0~3300,����0~3.3V
void DAC2_Set_Vol(u16 vol)
{
	double temp=vol;
	temp/=1000;
	temp=temp*4096/3.3;
    HAL_DAC_SetValue(&DAC_Handler,DAC_CHANNEL_2,DAC_ALIGN_12B_R,temp);//12λ�Ҷ������ݸ�ʽ����DACֵ
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(DAC1_Set_Vol, set dac1 value);
FINSH_FUNCTION_EXPORT(DAC2_Set_Vol, set dac2 value);
#endif
