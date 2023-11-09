#include "led.h"

//初始化PC13和PA5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;                     
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);    

	GPIO_InitStructure.GPIO_Pin = LED_PIN;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOC, LED_PIN); 						 //PC13 输出高 
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


void Buzzer_Init(void)//蜂鸣器初始化
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	    		 //端口配置, 推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	
	GPIO_SetBits(GPIOB,GPIO_Pin_12);//拉高
}
void Buzzer_on(void)
{
  GPIO_ResetBits(GPIOB,GPIO_Pin_12);
}

void Buzzer_off(void)
{ 
 GPIO_SetBits(GPIOB,GPIO_Pin_12);
}


void relay_init(void)
{
  GPIO_InitTypeDef     GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_10);
}          

void relay_on(void)
{
  GPIO_SetBits(GPIOA,GPIO_Pin_10);
}

void relay_off(void)
{ 
  GPIO_ResetBits(GPIOA,GPIO_Pin_10);
}
