#ifndef DHT11_H
#define DHT11_H
#include "main.h"
#include "iobit.h"

#define CPU_FREQUENCY_MHZ    80		// STM32ʱ����Ƶ
//IO��������
//IO��������
#define DHT11_IO_IN()  {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=8<<0;}
#define DHT11_IO_OUT() {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=3<<0;}
////IO��������											   
#define	DHT11_DQ_OUT PBout(0) //���ݶ˿�	PB0
#define	DHT11_DQ_IN  PBin(0)  //���ݶ˿�	PB0


uint8_t DHT11_Init(void);//��ʼ��DHT11
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi);//��ȡ��ʪ��
uint8_t DHT11_Read_Byte(void);//����һ���ֽ�
uint8_t DHT11_Read_Bit(void);//����һ��λ
uint8_t DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11   
void delay_us(uint32_t udelay);

void DHT11_Show_Data(void);
#endif
