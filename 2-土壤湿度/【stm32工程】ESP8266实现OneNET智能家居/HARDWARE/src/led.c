 
/*-------------------------------------------------*/
/*         	                                       */
/*           	 LED（补光灯）控制           	    */
/*                                                 */
/*-------------------------------------------------*/

// 硬件连接：
// PC13  LED0

#include "stm32f10x.h"  //包含需要的头文件
#include "led.h"        //包含需要的头文件

extern u8 fan;

extern u8 shui;

/*-------------------------------------------------*/
/*函数名：初始化LED函数,PC13,共阳极连接 	    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //端口配置, 推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	
	LED_Off();
}

/*-------------------------------------------------*/
/*函数名：LED开启                                  */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED_On(void)
{			
	GPIO_ResetBits(GPIOC, LED_PIN); 						 //PC13 输出低
} 


/*-------------------------------------------------*/
/*函数名：LED关闭                                  */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED_Off(void)
{		
	GPIO_SetBits(GPIOC, LED_PIN); 						 //PC13 输出高
}


/*-------------------------------------------------*/
/*函数名：蜂鸣器初始化函数,PB12,低电平触发 	    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void Buzzer_Init(void)//蜂鸣器初始化
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //端口配置, 推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz

}
void Buzzer_on(void)
{
  GPIO_ResetBits(GPIOA,GPIO_Pin_5);
}

void Buzzer_off(void)
{ 
 GPIO_SetBits(GPIOA,GPIO_Pin_5);
}

/*-------------------------------------------------*/
/*函数名：继电器初始化函数,PA10,               	    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void relay_init(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_15);
    GPIO_SetBits(GPIOB,GPIO_Pin_9);
    fan = 0;
    shui =0;
}          


void relay_fan_off(void)
{
    printf("fan  关\r\n");
	 GPIO_SetBits(GPIOB,GPIO_Pin_15);
     fan =0;
}

void relay_fan_on(void)
	
{ 
    printf("fan  开\r\n");
    GPIO_ResetBits(GPIOB,GPIO_Pin_15);
     fan = 1;
}



void relay_shui_off(void)
{
    printf("shui  关\r\n");
	GPIO_SetBits(GPIOB,GPIO_Pin_9);
    shui = 0;
 
}

void relay_shui_on(void)
{ 
     printf("shui  开\r\n");
    GPIO_ResetBits(GPIOB,GPIO_Pin_9);
    shui =1;
}



void relay_on(void)
{
	 GPIO_ResetBits(GPIOA,GPIO_Pin_10);
 
}

void relay_off(void)
	
{ 
  GPIO_SetBits(GPIOA,GPIO_Pin_10);
}

/*-------------------------------------------------*/
/*函数名：继电器初始化函数,PA9,               	    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void relay1_init(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_9);
}          

void relay1_on(void)
{
	 GPIO_ResetBits(GPIOA,GPIO_Pin_9);
 
}

void relay1_off(void)
	
{ 
  GPIO_SetBits(GPIOA,GPIO_Pin_9);
}
/*-------------------------------------------------*/
/*函数名：继电器初始化函数,PA8,               	    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void relay2_init(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}          

void relay2_on(void)
{
	 GPIO_ResetBits(GPIOA,GPIO_Pin_8);
 
}

void relay2_off(void)
	
{ 
  GPIO_SetBits(GPIOA,GPIO_Pin_8);
}

/*-------------------------------------------------*/
/*函数名：继电器初始化函数,PB15,               	    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void relay3_init(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}          

void relay3_on(void)
{
	 GPIO_ResetBits(GPIOB,GPIO_Pin_15);
 
}

void relay3_off(void)
	
{ 
  GPIO_SetBits(GPIOB,GPIO_Pin_15);
}
