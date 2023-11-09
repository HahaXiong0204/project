#ifndef DHT11_H
#define DHT11_H
#include "main.h"
#include "iobit.h"

#define CPU_FREQUENCY_MHZ    80		// STM32时钟主频
//IO方向设置
//IO方向设置
#define DHT11_IO_IN()  {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=8<<0;}
#define DHT11_IO_OUT() {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=3<<0;}
////IO操作函数											   
#define	DHT11_DQ_OUT PBout(0) //数据端口	PB0
#define	DHT11_DQ_IN  PBin(0)  //数据端口	PB0


uint8_t DHT11_Init(void);//初始化DHT11
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi);//读取温湿度
uint8_t DHT11_Read_Byte(void);//读出一个字节
uint8_t DHT11_Read_Bit(void);//读出一个位
uint8_t DHT11_Check(void);//检测是否存在DHT11
void DHT11_Rst(void);//复位DHT11   
void delay_us(uint32_t udelay);

void DHT11_Show_Data(void);
#endif
