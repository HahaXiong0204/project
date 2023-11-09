#ifndef _PUBLIC_H
#define _PUBLIC_H

#include "main.h"

#define Device_ID   2002
extern uint16_t  ID_Cart;
extern uint8_t i;

// ============================ ���� =========================
extern char Publish_Topc[64];
extern char Submit_Topc[64];
extern char Publish_Topc_ID[64];   // ��id���û�׼����
extern char Submit_Topc_ID[64];
extern const char *Publish_Buf[];  // ����������
extern const char *topics[];      // ���ĵ�����
extern const char *topics_id[];
// ==============================================================


// =========================== wifi ==========================
extern uint8_t cmd_flag;    // �����ӡ��־λ
extern uint8_t net_connect;            // �����Ƿ����ӵı�־λ
extern uint8_t rx_buff[1024];   // ��������
extern uint16_t rx_cnt;		// ��������ĸ���
extern uint8_t again_num;   // �ط����ݵ�����
extern uint8_t at_th_size;   // ��ֵ�����ݴ�С
extern char at_th_buff[56];  // ��ֵ������
// ============================================================

extern uint8_t beep_flag;    // 1 warning1   2 Error1
extern  uint8_t user_uing;    // �û�ʹ�ñ�־λ

// ========================== ���������� ==========================
#define ADC_MAX 	40
extern uint32_t adc_value[ADC_MAX];			//adc���ݻ�����
extern double temperature;               //�¶�
extern double humidity;                  //ʪ��
extern uint32_t Lighting,Turbidity,Gas;   //���գ��Ƕȣ���������� 
extern double voltage;    // ��ѹ
extern uint16_t speed;  //����ٶ�
// ================================================================


// ============================= ����������ֵ ====================
extern int TH_Temp;   //  �¶���ֵ
extern int TH_Hum;   // ʪ����ֵ
extern int TH_Gas;   // ������ֵ
extern int TH_light;   // ����ǿ����ֵ
// ==============================================================


extern uint8_t mode;

#endif
