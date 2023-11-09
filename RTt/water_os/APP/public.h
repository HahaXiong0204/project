#ifndef _PUBLIC_H
#define _PUBLIC_H

#include "rtthread.h"
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
extern uint8_t again_mode;   // �ط���־��
extern uint8_t at_th_size;   // ��ֵ�����ݴ�С
extern char at_th_buff[64];  // ��ֵ������

extern uint8_t user_mode;   // 0 �Զ�  1 �ֶ�
extern uint8_t state; 		   // �Ƿ�λ  1 ��ͣ  2 ����
// ============================================================

// ========================== ���������� ==========================
#define ADC_MAX 	40
extern uint32_t adc_value[ADC_MAX];			//adc���ݻ�����
extern double temperature;               //�¶�
extern double humidity;                  //ʪ��
extern uint32_t Lighting,Turbidity,Gas;   //���գ��Ƕȣ���������� 
extern double voltage;    // ��ѹ
extern uint16_t speed;     // ����ٶ�
extern uint8_t people;   // ���崫����
extern uint8_t Led_position;         // led�ĵ�λ 
extern uint8_t machinery_position;    // �����λ

extern uint8_t tem_flag;     // �¶ȱ�־λ
extern uint8_t Hum_flag;	  // ʪ�ȱ�־��
extern uint8_t Gas_flag;	  // ������־λ
extern uint8_t light_flag;    // ���ձ�־λ

// ================================================================


// ============================= ����������ֵ ====================
extern int TH_Temp;   //  �¶���ֵ
extern int TH_Hum;   // ʪ����ֵ
extern int TH_Gas;   // ������ֵ
extern int TH_light;   // ����ǿ����ֵ
// ==============================================================


// ============================ user start ==============================
extern uint8_t beep_flag;    // 1 warning1   2 Error1
extern  uint8_t user_uing;    // �û�ʹ�ñ�־λ
// ============================ user end ==============================



#endif
