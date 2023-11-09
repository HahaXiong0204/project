#ifndef __DS18B20_2_H
#define __DS18B20_2_H
#include "sys.h"
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//DS18B20_2��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//IO��������
#define DS18B20_2_IO_IN()  {GPIOH->MODER&=~(3<<(9*2));GPIOH->MODER|=0<<9*2;}	//PB5����ģʽ
#define DS18B20_2_IO_OUT() {GPIOH->MODER&=~(3<<(9*2));GPIOH->MODER|=1<<9*2;} 	//PB5���ģʽ
 
////IO��������											   
#define	DS18B20_2_DQ_OUT PHout(9) //���ݶ˿�	PB12
#define	DS18B20_2_DQ_IN  PHin(9)  //���ݶ˿�	PB12 
   	
uint8_t DS18B20_2_Init(void);			//��ʼ��DS18B20_2
short DS18B20_2_Get_Temp(void);	//��ȡ�¶�
void DS18B20_2_Start(void);		//��ʼ�¶�ת��
void DS18B20_2_Write_Byte(uint8_t dat);//д��һ���ֽ�
uint8_t DS18B20_2_Read_Byte(void);		//����һ���ֽ�
uint8_t DS18B20_2_Read_Bit(void);		//����һ��λ
uint8_t DS18B20_2_Check(void);			//����Ƿ����DS18B20_2
void DS18B20_2_Rst(void);			//��λDS18B20_2 
#endif
