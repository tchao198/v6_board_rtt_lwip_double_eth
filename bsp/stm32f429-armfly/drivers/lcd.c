#include "lcd.h"

uint16_t DB[] = {GPIO_PIN_5,GPIO_PIN_4,GPIO_PIN_3,GPIO_PIN_7,GPIO_PIN_6,GPIO_PIN_5,GPIO_PIN_4,GPIO_PIN_1};

void LcdCommandWrite(int value) 
{
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);
	int i = 0;
	for (i=0; i <= 7; i++) 
	{
		switch(i){
			case 0:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_1, value & 01);
				break;
			case 1:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_2, value & 01);
				break;
			case 2:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_3, value & 01);
				break;
			case 3:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, value & 01);
				break;
			case 4:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, value & 01);
				break;
			case 5:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_6, value & 01);
				break;
			case 6:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_7, value & 01);
				break;
			case 7:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_8, value & 01);
				break;
			default:
				break;
		}
		 value >>= 1;
	}
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_9, GPIO_PIN_RESET);
	delay_us(1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_9, GPIO_PIN_SET);
	delay_us(1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_9, GPIO_PIN_RESET);
	delay_us(1);
}

void LcdDataWrite(int value) 
{
	int i = 0;
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_11, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_10, GPIO_PIN_RESET);
	for (i=0; i <= 7; i++) 
	{
		switch(i){
			case 0:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_1, value & 01);
				break;
			case 1:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_2, value & 01);
				break;
			case 2:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_3, value & 01);
				break;
			case 3:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, value & 01);
				break;
			case 4:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_5, value & 01);
				break;
			case 5:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_6, value & 01);
				break;
			case 6:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_7, value & 01);
				break;
			case 7:
				HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_8, value & 01);
				break;
			default:
				break;
		}
		 value >>= 1;
	}
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_9, GPIO_PIN_RESET);
	delay_us(1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_9, GPIO_PIN_SET);
	delay_us(1);
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_9, GPIO_PIN_RESET);
	delay_us(1);
}

void lcdStrWrite(char *s)
{
	int i=0;
	while(s[i]!='\0')
	{
		LcdDataWrite(s[i]);
		delay_ms(1);
		i++;
	}
}

void lcdDecimalWrite(int value)
{
	int i=3;
	char s[5] = {'0','0','0','0','\0'};
	s[4] = '\0';
	
	while(value!=0)
	{
		s[i--] = value%10 + 48;
		value /= 10;
	}
	lcdStrWrite(s);
}

void lcd1602_init(void)
{
	 GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOJ_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_SET);

  /*Configure GPIO pins */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);
	
	
	LcdCommandWrite(0x38);  // ???8-bit??,2???,5x7????                     
	delay_ms(10);                      
	LcdCommandWrite(0x38);  // ???8-bit??,2???,5x7????                        
	delay_ms(100);                      
	LcdCommandWrite(0x38);  // ???8-bit??,2???,5x7????                        
	delay_ms(100);                      
	LcdCommandWrite(0x06);  // ??????
													 // ????,??????
	delay_ms(100);                      
	LcdCommandWrite(0x0C);  // ????
													 // ?????,????,???
	delay_ms(100);                      
	LcdCommandWrite(0x01);  // ????,??????  
	delay_ms(100);                      
	LcdCommandWrite(0x80);  // ????
	delay_ms(100); 
	
	LcdCommandWrite(0x01);  // ????,??????  
  delay_ms(100); 
  LcdCommandWrite(0x80+5); 
  delay_ms(100); 
	lcdStrWrite("Welcome");
//  LcdDataWrite('W');
//	delay_ms(100); 
//  LcdDataWrite('e');
//	delay_ms(100); 
//  LcdDataWrite('l');
//	delay_ms(100); 
//  LcdDataWrite('c');
//	delay_ms(100); 
//  LcdDataWrite('o');
//	delay_ms(100); 
//  LcdDataWrite('m');
//	delay_ms(100); 
//  LcdDataWrite('e');
	delay_ms(100); 
	LcdCommandWrite(0xC0+4);
	delay_ms(100);    
	lcdStrWrite("Ala baby!");
//  LcdDataWrite('F');
//	delay_ms(100); 
//  LcdDataWrite('i');
//	delay_ms(100); 
//  LcdDataWrite('b');
//	delay_ms(100); 
//  LcdDataWrite('e');
//	delay_ms(100); 
//  LcdDataWrite('r');
//	delay_ms(100); 
//  LcdDataWrite(' ');
//	delay_ms(100); 
//  LcdDataWrite('f');
//	delay_ms(100); 
//	LcdDataWrite('e');
//	delay_ms(100); 
//  LcdDataWrite('n');
//	delay_ms(100); 
//  LcdDataWrite('c');
//	delay_ms(100); 
//  LcdDataWrite('e');
	delay_ms(100);  
}
