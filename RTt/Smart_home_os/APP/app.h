#ifndef _APP_H
#define _APP_H

#include "main.h"
#include "public.h"
#include "wifi.h"
#include "onenet.h"
#include "dht11.h"
#include "delay.h"
#include "24c02.h"
#include "PCF8574.h"
#include "sdram.h"
#include "rc522.h"
#include "malloc.h"


void Mx_Init(void);  // ��ʼ������

void lcd_show(void *pargem); // ��ʾ������lcd��Ļ��
void collect_data(void *pargem);   // �ɼ�����������

void Logical_code_entry(void *pargem);   // �߼��ж�
void at_sava_th(void);    // ������ֵ����
void read_rfid(void *pargem);   // ��rfid

void resest_wifi(void);      // ��������wifi
void resest_server(void);   // ��������������


// ����LED��λ
void Led_pwm_mode(uint8_t mode);
// ���õ����λ
void machinery_pwm_mode(uint8_t mode);






#endif
