#ifndef __DS18B20_2_H
#define __DS18B20_2_H
#include "sys.h"
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//DS18B20_2驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//IO方向设置
#define DS18B20_2_IO_IN()  {GPIOH->MODER&=~(3<<(9*2));GPIOH->MODER|=0<<9*2;}	//PB5输入模式
#define DS18B20_2_IO_OUT() {GPIOH->MODER&=~(3<<(9*2));GPIOH->MODER|=1<<9*2;} 	//PB5输出模式
 
////IO操作函数											   
#define	DS18B20_2_DQ_OUT PHout(9) //数据端口	PB12
#define	DS18B20_2_DQ_IN  PHin(9)  //数据端口	PB12 
   	
uint8_t DS18B20_2_Init(void);			//初始化DS18B20_2
short DS18B20_2_Get_Temp(void);	//获取温度
void DS18B20_2_Start(void);		//开始温度转换
void DS18B20_2_Write_Byte(uint8_t dat);//写入一个字节
uint8_t DS18B20_2_Read_Byte(void);		//读出一个字节
uint8_t DS18B20_2_Read_Bit(void);		//读出一个位
uint8_t DS18B20_2_Check(void);			//检测是否存在DS18B20_2
void DS18B20_2_Rst(void);			//复位DS18B20_2 
#endif
