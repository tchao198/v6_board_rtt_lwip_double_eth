#ifndef __LCD_H
#define __LCD_H
#include "stm32f4xx_hal.h"
#include "delay.h"
void lcd1602_init(void);
void LcdDataWrite(int value);
void LcdCommandWrite(int value);
void lcdStrWrite(char *s);
void lcdDecimalWrite(int value);
#endif
