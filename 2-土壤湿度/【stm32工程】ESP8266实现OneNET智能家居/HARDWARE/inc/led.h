
#ifndef __LED_H
#define __LED_H

#define LED_PIN		GPIO_Pin_13



void LED_Init(void);               //��ʼ��	
void LED_On(void);              //��������LED
void LED_Off(void);             //Ϩ������LED

void Buzzer_Init(void);//��������ʼ��

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
