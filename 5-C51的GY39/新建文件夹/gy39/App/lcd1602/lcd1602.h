#ifndef _lcd1602_H
#define _lcd1602_H

#include "public.h"

#include <reg52.h>


//引脚配置：
#define LCD_DataPort P2
sbit LCD_RS = P0^4;
sbit LCD_RW = P0^3;
sbit LCD_EN = P0^2;
//用户调用函数：
void LCD_Init();
void LCD_ShowChar(unsigned char Line,unsigned char Column,char Char);
void LCD_ShowString(unsigned char Line,unsigned char Column,char *String);
void LCD_ShowNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length);
void LCD_ShowSignedNum(unsigned char Line,unsigned char Column,int Number,unsigned char Length);
void LCD_ShowHexNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length);
void LCD_ShowBinNum(unsigned char Line,unsigned char Column,unsigned int Number,unsigned char Length);

#endif