#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED_PIN		GPIO_Pin_13

      



void LED_Init(void);               //��ʼ��	
void LED_On(void);              //��������LED
void LED_Off(void); 
void Buzzer_Init(void);//��������ʼ��
void Buzzer_on(void);
void Buzzer_off(void);

void relay_init(void); //�̵���
void relay_on(void);
void relay_off(void);
#endif
