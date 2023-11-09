#ifndef _ds18b20_H
#define _ds18b20_H

#include "ds18b20.h" 
#include "system.h" 
/*  DS18B20ʱ�Ӷ˿ڡ����Ŷ��� */
#define DS18B20_PORT 			GPIOB  
#define DS18B20_PIN 			(GPIO_Pin_3)
#define DS18B20_PORT_RCC		RCC_APB2Periph_GPIOB

#define u8 unsigned char
#define u16 unsigned int


////IO��������											   
#define	DS18B20_DQ_OUT PGout(3) //���ݶ˿�	PB3
#define	DS18B20_DQ_IN  PGin(3)  //���ݶ˿�	PB3
   	
u8 DS18B20_Init(void);			//��ʼ��DS18B20
float DS18B20_GetTemperture(void);	//��ȡ�¶�
u16 DS18B20_GetTemperU16(void);	//16���Ƹ�ʽ��ȡ�¶� added by zhang
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Reset(void);			//��λDS18B20   




#endif
