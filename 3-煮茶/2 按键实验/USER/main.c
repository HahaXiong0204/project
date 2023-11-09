

/* /  
瑞仿电子	  ELH    enlighten  sunny
STM32 开发板 库函数部分	  */

//按键控制LED灯实验  
//板子上的按键 是S2 
//此实验 通过S2按键按下 led灯亮 按键松开 LED灭



#include  "delay.h"
#include  "led.h"
#include  "key.h"


int main(void)
   {

   	delay_init();	     //延时函数初始化	  
	LED_Init();		  	//初始化与LED连接的硬件接口
	KEY_Init();         //初始化按键
   while(1)
   {
 
        if(KEY0==0)
		  {
		    GPIO_ResetBits(GPIOA,GPIO_Pin_1);		  //给PA1置0  点亮LED
		  }

		   GPIO_SetBits(GPIOA,GPIO_Pin_1);		      //给PA1置1  关闭LED

          }
   }

