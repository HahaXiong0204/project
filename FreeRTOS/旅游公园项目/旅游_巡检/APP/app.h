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
#include "key.h"
#include "fontupd.h"
#include "w25qxx.h"
#include "text.h"


 

extern char at_data_arr[256];
extern struct AT24CDATA at_data_struct;    // ����Ľṹ��


void Mx_Init(void);  // ��ʼ������

void Net_Connect_entery(void *pargem);   // ���紦����


void at_sava_th(void);    // ������ֵ����

#if RFID_FLAG
void read_rfid(void *pargem);   // ��rfid
#endif

void resest_wifi(void);      // ��������wifi
void resest_server(void);   // ��������������


// ����LED��λ
void Led_pwm_mode(uint8_t mode);
// ���õ����λ
void machinery_pwm_mode(uint8_t mode);


// ���͵����ͻ�����
void Send_queue(char *msg);


/*  ͨ������2 ���͸�zigbee */
void Send_Zigbee(void);
/*  ͨ������6 ���͸����� */
void Send_tianwen(uint8_t mode);

// =============================  ������ ============================
/*�ȴ�������*/
void Wait_send_mutex(void);
/*�ͷŷ�����*/
void Release_send_mutex(void);
/*�ȴ�iic��spi��*/
void Wait_iic_spi_mutex(void);
/*�ͷ�iic��spi��*/
void Release_iic_spi_mutex(void);
/*�ȴ�������*/
void Wait_reve_mutex(void);
/*�ͷŽ�����*/
void Release_reve_mutex(void);
	
// ======================================================================	

#endif
