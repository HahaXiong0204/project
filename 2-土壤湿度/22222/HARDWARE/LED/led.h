#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED_PIN		GPIO_Pin_13

      



void LED_Init(void);               //初始化	
void LED_On(void);              //点亮所有LED
void LED_Off(void); 
void Buzzer_Init(void);//蜂鸣器初始化
void Buzzer_on(void);
void Buzzer_off(void);

void relay_init(void); //继电器
void relay_on(void);
void relay_off(void);
#endif
