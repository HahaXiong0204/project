
#ifndef __TFT_H
#define __TFT_H
#include "stm32f10x.h"


void TFT_CS(u8 d);

void Lcd_Init(void);


void Lcd_Write_Data(u16 data);								 //д16λ����

void Address_set(u8 x1,u16 y1,u8 x2,u16 y2,u8 xq,u16 yq);    //������ʾ��Χ




//��Ļ�ߴ�
#define TFT_XMAX       239
#define TFT_YMAX       319




#endif






