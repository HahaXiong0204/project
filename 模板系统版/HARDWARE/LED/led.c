#include "led.h"

//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////


void LED_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;					 //定义GPIO结构体

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能PA端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;			 //LED-->PA1端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.1
    GPIO_SetBits(GPIOA, GPIO_Pin_1);						 //PA.A 输出高

}




/*-------------------------------------------------*/
/*函数名：蜂鸣器初始化函数,PB12,低电平触发 	    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Buzzer_Init(void)//蜂鸣器初始化
{

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //端口配置, 推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz

    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}
void Buzzer_on(void)
{
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

void Buzzer_off(void)
{
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

/*-------------------------------------------------*/
/*函数名：继电器初始化函数,PA10,               	    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void relay_init(void)
{
    GPIO_InitTypeDef     GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_10);
}

void relay_on(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_10);

}

void relay_off(void)

{
    GPIO_SetBits(GPIOA, GPIO_Pin_10);
}

/*-------------------------------------------------*/
/*函数名：继电器初始化函数,PB5,               	    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void relay1_init(void)
{
    GPIO_InitTypeDef     GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}

void relay1_on(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_5);

}

void relay1_off(void)

{
    GPIO_SetBits(GPIOB, GPIO_Pin_5);
}
/*-------------------------------------------------*/
/*函数名：继电器初始化函数,PB14,               	    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void relay2_init(void)
{
    GPIO_InitTypeDef     GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

void relay2_on(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);

}

void relay2_off(void)

{
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

/*-------------------------------------------------*/
/*函数名：继电器初始化函数,PB15,               	    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void relay3_init(void)
{
    GPIO_InitTypeDef     GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void relay3_on(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);

}

void relay3_off(void)

{
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}
