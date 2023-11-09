

#include "key.h"




void KEY_Init(void)
{
GPIO_InitTypeDef GPIO_InitStructure;	 //结构体定义
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD,ENABLE);	//开启GPIOA GPIOD时钟
GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;								   //IO设置为PD2
GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;		   //设置成上拉输入
GPIO_Init(GPIOD,&GPIO_InitStructure);	           //初始化GPIOD2


//初始化 GPIOA.8	  下拉输入
GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA8设置成输入，默认上拉	  
GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0

}












