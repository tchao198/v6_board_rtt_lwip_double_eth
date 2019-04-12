/*yj12864.h*/
#ifndef _YJ12864_H
#define _YJ12864_H
#include "sys.h"

//YJ-G12864-97
//1=SDA  2=SCK   3=CS  4=CD(RS)  5=RST  6=VDD  7=VSS  8=K  9=A
//For STM32F103      

//#define  RS     PBout(2)
#define  RST     PBout(3)
#define  CS     PEout(4)
#define  SDA     PBout(4)
#define  SCK     PBout(5)

extern void Lcd_init(void);
extern u8 Read_data(void);
extern void Write_data(u8 data);
extern void Write_command(u8 com);

extern void Lcd_pos(u8 x,u8 y);
extern void Clear_screen(void);
extern void Disp_char(u8 x, u8 y, u8 z);
extern void Disp_string(u8 x,u8 y,u8 *z);
extern void Disp_num(u8 x,u8 y,s16 num); 

extern void Clear_GDRAM(void);
extern void Draw_point(u8 x,u8 y,u8 color);


#endif
