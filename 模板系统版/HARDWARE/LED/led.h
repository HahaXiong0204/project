#ifndef __LED_H
#define __LED_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
#define LED0 PAout(A)// PA1


void LED_Init(void);//��ʼ��




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
