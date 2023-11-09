#ifndef __1602_H__
#define __1602_H__

#include "stm32f10x.h"

#define uchar unsigned char
#define uint unsigned int

extern uchar  AsciiCode[];
void Init1602(void);					 
void LCD_Write_String(uchar x1,uchar x,uchar *p);
void WrByte1602(uchar x,uchar y,uchar dat);
#endif
