#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED PCout(13)	// LED�ӿ�
#define Buzzer PBout(10)	
#define Buzzer_ON  0
#define Buzzer_OFF 1
#define LED_OFF		1
#define LED_ON		0

void LED_Init(void);//��ʼ��
void Buzzer_Init(void);//��������ʼ��
void Relay_Init(void);
void Relay_Fan_On(void);
void Relay_Fan_Off(void);
			    
#endif
