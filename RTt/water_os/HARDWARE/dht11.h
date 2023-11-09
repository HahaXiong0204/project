#ifndef DHT11_H
#define DHT11_H
#include "main.h"
#include "sys.h"
#include "delay.h"

//IO��������
//IO��������
#define DHT11_IO_IN()  {GPIOG->MODER &= ~(GPIO_MODER_MODER14_0);GPIOG->MODER &= ~(GPIO_MODER_MODER14_1);} // ����Ϊ���ģʽ
// PC10 IN  MODE
#define DHT11_IO_OUT() {GPIOG->MODER &= ~(GPIO_MODER_MODER14); GPIOG->MODER |= GPIO_MODER_MODER14_0;} // PC10 OUT MODE
////IO��������
#define	DHT11_DQ_OUT PGout(14) //���ݶ˿�	PB0
#define	DHT11_DQ_IN  PGin(14)  //���ݶ˿�	PB0


uint8_t DHT11_Init(void);//��ʼ��DHT11
uint8_t DHT11_Read_Data(double *temp, double *humi); //��ȡ��ʪ��
uint8_t DHT11_Read_Byte(void);//����һ���ֽ�
uint8_t DHT11_Read_Bit(void);//����һ��λ
uint8_t DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11


void DHT11_Show_Data(void);
#endif

