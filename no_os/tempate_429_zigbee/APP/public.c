#include "public.h"


uint8_t i;


// =========================== wifi ==========================
uint8_t cmd_flag = 0;    // �����ӡ��־λ
uint8_t net_connect = 0;            // �����Ƿ����ӵı�־λ
uint8_t rx_buff[1024];
uint16_t rx_cnt = 0;
uint8_t again_num = 0;


// ============================================================

uint8_t beep_flag = 0;    // 1 warning1   2 Error1

// ========================== ���������� start ==========================
uint32_t adc_value[ADC_MAX];              //adc���ݻ�����
double temperature=0;               //�¶�
double humidity=0;                  //ʪ��
uint32_t Lighting=0,Turbidity=0,Gas=0;   //���գ��Ƕ�
double voltage = 0;  // ��ѹ
uint16_t speed= 0; // �ٶ�
// ===========================���������� end =====================================

uint8_t user_uing = 0;    // �û�ʹ�ñ�־λ


// ============================= ����������ֵ start====================
int TH_Temp = 36;   //  �¶���ֵ
int TH_Hum = 60;   // ʪ����ֵ
int TH_Gas = 80;   // ������ֵ
int TH_light = 30;   // ����ǿ����ֵ
// ==============================����������ֵ end================================



