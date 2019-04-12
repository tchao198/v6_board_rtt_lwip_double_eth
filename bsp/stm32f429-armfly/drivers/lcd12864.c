/*yj12864.c*/
#include "lcd12864.h"
#include "delay.h"
#include "stm32f4xx_hal.h"

const u8 zimu[][6] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },   // sp  ASCII = 32
    { 0x00, 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
    { 0x00, 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
    { 0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
    { 0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
    { 0x00, 0x62, 0x64, 0x08, 0x13, 0x23 },   // %
    { 0x00, 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
    { 0x00, 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
    { 0x00, 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
    { 0x00, 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
    { 0x00, 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
    { 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
    { 0x00, 0x00, 0x00, 0xA0, 0x60, 0x00 },   // ,
    { 0x00, 0x08, 0x08, 0x08, 0x08, 0x08 },   // -
    { 0x00, 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
    { 0x00, 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
    { 0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
    { 0x00, 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
    { 0x00, 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
    { 0x00, 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
    { 0x00, 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
    { 0x00, 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
    { 0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
    { 0x00, 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
    { 0x00, 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
    { 0x00, 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
    { 0x00, 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
    { 0x00, 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
    { 0x00, 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
    { 0x00, 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
    { 0x00, 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
    { 0x00, 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
    { 0x00, 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
    { 0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C },   // A
    { 0x00, 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
    { 0x00, 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
    { 0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
    { 0x00, 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
    { 0x00, 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
    { 0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
    { 0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
    { 0x00, 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
    { 0x00, 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
    { 0x00, 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
    { 0x00, 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
    { 0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
    { 0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
    { 0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
    { 0x00, 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
    { 0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
    { 0x00, 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
    { 0x00, 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
    { 0x00, 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
    { 0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
    { 0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
    { 0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
    { 0x00, 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
    { 0x00, 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
    { 0x00, 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
    { 0x00, 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
    { 0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55 },   // 55
    { 0x00, 0x00, 0x41, 0x41, 0x7F, 0x00 },   // ]
    { 0x00, 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
    { 0x00, 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
    { 0x00, 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
    { 0x00, 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
    { 0x00, 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
    { 0x00, 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
    { 0x00, 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
    { 0x00, 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
    { 0x00, 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
    { 0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C },   // g
    { 0x00, 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
    { 0x00, 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
    { 0x00, 0x40, 0x80, 0x84, 0x7D, 0x00 },   // j
    { 0x00, 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
    { 0x00, 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
    { 0x00, 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
    { 0x00, 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
    { 0x00, 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
    { 0x00, 0xFC, 0x24, 0x24, 0x24, 0x18 },   // p
    { 0x00, 0x18, 0x24, 0x24, 0x18, 0xFC },   // q
    { 0x00, 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
    { 0x00, 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
    { 0x00, 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
    { 0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
    { 0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
    { 0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
    { 0x00, 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
    { 0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C },   // y
    { 0x00, 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z 
    { 0x14, 0x14, 0x14, 0x14, 0x14, 0x14 }    // horiz lines     
};

void Write_8bits(u8 W_bits)
{
 u8 i,Temp_data;
 for(i=0; i<8; i++)//??????,????8bits??????
 {
  Temp_data = W_bits;
  Temp_data <<= i;//???????
  if((Temp_data&0x80)==0)  //????????0
  {
   SDA=0;// SID = 0;
   SCK=0;//SCLK = 0;//??????????????
   SCK=1;// SCLK = 1;//????,????????
  }
  else         //????1
  {
   SDA=1;//SID = 1;
   SCK=0;//SCLK = 0;
   SCK=1; //SCLK = 1;
  }
 }
}


void W_1byte(u8 RW, u8 RS, u8 W_data)
{ 
 u8 H_data,L_data,S_ID = 0xf8;  //11111RWRS0
 if(RW == 0)           //写LCD
 {
   S_ID &=~ 0x04;
 }
 else     //if(RW==1)  //读LCD
 {
   S_ID |= 0X04;
 }
 if(RS == 0)          //命令
 {
   S_ID &=~ 0x02;
 }                                                                                                                               
 else     //if(RS==1) //数据
 {
   S_ID |= 0X02;
 }

 H_data = W_data;
 H_data &= 0xf0;   	//屏蔽低四位数据
 L_data = W_data;   //xxxx0000格式
 L_data &= 0x0f;  	//屏蔽高四位数据
 L_data <<= 4;   		//xxxx0000
 CS=1;							//CS = 1; 使能发送
 Write_8bits(S_ID);   //发送命令字S_ID
 Write_8bits(H_data); //发送高四位数据H_data
 Write_8bits(L_data); //发送低四位数据L_data
 CS=0;                //CS=0   
}   
//写字符串
 void LCD_write_string(u8 *s)
{
  for(u8 i=0;s[i]!='\0';i++){
      W_1byte(0,1,s[i]);
			delay_ms (10);
	}
}

//浮点转换字符串
int ftoa(char *str, float num, int n) 
{
    int     sumI;
    float   sumF;
    int     temp;
    int     count = 0;
    char *p;
    char *pp;
    if(str == NULL) return -1;
    p = str;
   
    if(num < 0)
    {
        num = 0 - num;
    }
    sumI = (int)num;    //sumI is the part of int
    sumF = num - sumI;  //sumF is the part of float
   
    do
    {
        temp = sumI % 10;
        *(str++) = temp+48;
    }while((sumI = sumI /10) != 0);
    pp = str;
    pp--;
    while(p < pp)
    {
        *p = *p + *pp;
        *pp = *p - *pp;
        *p = *p -*pp;
        p++;
        pp--;
    }
    *(str++) = '.';
   
    do
    {
        temp = (int)(sumF*10);
        *(str++) = temp+48;
        if((++count) == n)
            break;
        sumF = sumF*10 - temp;
    }while(!(sumF > -0.000001 && sumF < 0.000001));
		*str='\0';
    return 0;
}


void Lcd_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

		/* GPIO Ports Clock Enable */
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOE_CLK_ENABLE();

		/*Configure GPIO pin Output Level */
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

		/*Configure GPIO pins */
		GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
		GPIO_InitStruct.Pin = GPIO_PIN_4;
		HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
		RST=0; /*低电平复位*/
    delay_ms(20);
    RST=1; /*复位完毕*/
    delay_ms(20);
		W_1byte(0,0,0x30);//功能设置8位数据，基本指令
		delay_ms(40);
//		W_1byte(0,0,0x02); //地址归为
//		delay_ms(40);
		W_1byte(0,0,0x06); //游标及显示有义一位
		delay_ms (40);
		W_1byte(0,0,0x0C); //显示状态 ON,游标OFF, 反白OFF
		delay_ms (40);
		W_1byte(0,0,0x01);  //清除显示
		delay_ms (40);
		W_1byte(0,0,0x91);//设置写入地址
		delay_ms (40);
//		W_1byte(0,1,97);// 写a
//		delay_ms (40);
//		W_1byte(0,1,98);// 写b
//		delay_ms (40);
//		W_1byte(0,1,99);// 写c
//		delay_ms (40);
		LCD_write_string("你好！左唐屹");

    
//    RST=0; /*低电平复位*/
//    delay_ms(20);
//    RST=1; /*复位完毕*/
//    delay_ms(20);
//    Write_command(0xe2); /*软复位*/
//    delay_ms(5);
//    Write_command(0x2c); /*升压步聚1*/
//    delay_ms(5);
//    Write_command(0x2e); /*升压步聚2*/
//    delay_ms(5);
//    Write_command(0x2f); /*升压步聚3*/
//    delay_ms(5);
//    Write_command(0x25); /*粗调对比度，可设置范围0x20～0x27*/
//    Write_command(0x81); /*微调对比度*/
//    Write_command(0x1A); /*微调对比度的值，可设置范围0x00～0x3f*/
//    Write_command(0xa2); /*1/9 偏压比（bias）*/
//    Write_command(0xc8); /*行扫描顺序：从上到下*/
//    Write_command(0xa0); /*列扫描顺序：从左到右*/
//    Write_command(0x40); /*起始行：第一行开始*/
//    Write_command(0xaf); /*开显示*/
//    delay_ms(10);
//    Write_command(0x30);
//    delay_ms(5);
//    Write_command(0x30);
//    delay_ms(10);
//    Write_command(0x0c);
//    
//    Clear_screen();
//		Disp_num(64,32,8);
}





//void Write_command(u8 com)
//{
//    u8 i;
//    CS=1;
//    for(i=0;i<8;i++)
//    {
//        SCK=0;
//        if(com&0x80) SDA=1;
//        else SDA=0;
//        delay_ms(2);
//        SCK=1;
//        delay_ms(2);
//        com=(com<<1);
//    }
//}
//void Write_data(u8 data)
//{
//    u8 i;
//    CS=1;
//    for(i=0;i<8;i++)
//    {
//        SCK=0;
//        if(data&0x80) SDA=1;
//        else SDA=0;
//        SCK=1;
//        data=(data<<1);
//    }
//}
//void Lcd_pos(u8 x,u8 y)
//{
//    Write_command(0xb0+y);               
//    Write_command(((x&0xf0)>>4)|0x10);  
//    Write_command((x&0x0f)|0x01);       
//}
//void Clear_screen(void)
//{
//    u8 i,j;
//    for(i=0;i<9;i++)
//    {
//        CS=1;
//        Write_command(0xb0+i);
//        Write_command(0x10);
//        Write_command(0x01);
//        for(j=0;j<132;j++)
//            Write_data(0x00);
//    }
//}
//void Disp_char(u8 x, u8 y, u8 z)
//{
//    
//    u8 j;
//    z -=32;
//    Lcd_pos(x,y);
//    for(j=0;j<6;j++)
//    {
//        Write_data(zimu[z][j]);
//    }
//}
//void Disp_string(u8 x,u8 y,u8 *z)     
//{
//    while(*z)
//    {
//        Disp_char(x, y, *z);
//        z++;
//        x +=6;
//        if(x>126)
//        {
//            x=0;
//            y++;
//        }
//    }
//}
//void Disp_num(u8 x,u8 y,s16 num)  
//{//s16 = signed int
//    u8 c,i,j,n[7]={0};  
//    if(num<0)
//    {
//        Disp_char(x,y,'-');
//        x +=6;
//        num=-num;
//    }
//    if(num==0)
//    {
//        Disp_char(x,y,'0');
//    }    
//    else
//    {
//        for(i=0;num>0;i++)
//        {
//            n[i]=num%10;
//            num/=10;
//        }
//        while(i--)
//        {
//            c=n[i]+16;
//            Lcd_pos(x,y);    
//            for(j=0;j<6;j++)
//            {
//                 Write_data(zimu[c][j]);
//            }       
//            x+=6;
//        }
//    }  
//}
//void Clear_GDRAM(void)
//{
//    u8 j,k;
//    Write_command(0x34);                              
//    for( j = 0 ; j < 32 ; j++ )
//    {
//        Write_command(0x80 + j);   // Y  
//        Write_command(0x80);       // X
//        for( k = 0 ; k < 32 ; k++ ) 
//        {// address auto ++
//            Write_data(0x00);
//        }
//    }      
//    Write_command(0x30);  
//}
//void Draw_point(u8 x,u8 y,u8 color)
//{//color = 0, 1
//    u8 X,Y,k,DH=0x00,DL=0x00; 
//    Write_command(0x34);   
//    if(y >= 0 && y <= 63 && x >= 0 && x <= 127) 
//    {
//        if(y < 32)
//        {   
//            X = 0x80 + (x >> 4);    //  x/16     128/16=8
//            Y = 0x80 + y; 
//        }
//        else
//        {
//            X = 0x88 + (x >> 4);
//            Y = 0x80 + (y - 32);
//        }
//    }
//    k = x%16;
//    switch(color)
//    {     
//        case 0: if(k < 8)  
//                    DH = ~(0x01 << (7 - k));  
//                else
//                    DL = ~(0x01 << (7 - (k % 8)));  
//                break;
//        case 1: if(k < 8)
//                    DH = (0x01 << (7 - k));  
//                else
//                    DL = (0x01 << (7 - (k % 8))); 
//                break;
//        default: break;  
//    }
//    Write_command(Y);  
//    Write_command(X);  
//    Write_data(DH);
//    Write_data(DL);
//    Write_command(0x36);
//    Write_command(0x30);      
//}