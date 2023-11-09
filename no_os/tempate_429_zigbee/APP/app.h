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
#include "malloc.h"
#include "rc522.h"

void show_zigbee(void);
void Mx_Init(void);  // ��ʼ������

void lcd_show(void); // ��ʾ������lcd��Ļ��
void collect_data(uint8_t mode);   // �ɼ�����������

void at_sava_th(void);
void resest_wifi(void);      // ��������wifi
void resest_server(void);   // ��������������










#endif
