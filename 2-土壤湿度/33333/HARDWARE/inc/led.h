#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED PCout(13)	// LED�ӿ�
#define Buzzer PCout(15)	
#define Buzzer_ON  1
#define Buzzer_OFF 0
#define LED_OFF		1
#define LED_ON		0

void LED_Init(void);//��ʼ��
void Buzzer_Init(void);//��������ʼ��
			    
#endif
