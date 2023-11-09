
#ifndef __LED_H
#define __LED_H

#define LED_PIN		GPIO_Pin_13



void LED_Init(void);               //初始化	
void LED_On(void);              //点亮所有LED
void LED_Off(void);             //熄灭所有LED

void Buzzer_Init(void);//蜂鸣器初始化

void Buzzer_on(void);


void Buzzer_off(void);

void relay_init(void);

void relay1_init(void);

void relay2_init(void);

void relay3_init(void);
 

void relay_on(void);

void relay_off(void);


void relay1_on(void);

void relay1_off(void);


void relay2_on(void);

void relay2_off(void);

void relay3_on(void);

void relay3_off(void);

#endif
