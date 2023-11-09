#include "public.h"


uint8_t i;


// =========================== wifi ==========================
uint8_t cmd_flag = 0;    // �����ӡ��־λ
uint8_t net_connect = 0;            // �����Ƿ����ӵı�־λ
uint8_t rx_buff[1024];
uint16_t rx_cnt = 0;
uint8_t again_num = 0;
uint8_t again_mode = 0;   // �ط���־��
uint8_t state = 0;    // �û�ģʽ
uint8_t user_uing = 0;    // �û�ʹ�ñ�־λ
uint8_t user_mode = 0;   // 0 �Զ�  1 �ֶ�
// ============================================================



// ========================== ���������� start ==========================
uint32_t adc_value[ADC_MAX];              //adc���ݻ�����
double temperature=0;               //�¶�
double humidity=0;                  //ʪ��
uint32_t Lighting=0,Turbidity=0,Gas=0;   //���գ��Ƕ�
double voltage = 0;  // ��ѹ
uint16_t speed = 0;     // ����ٶ�
uint8_t people = 0;   // ���崫����
// ===========================���������� end =====================================




// ============================= ����������ֵ start====================
int TH_Temp = 48;   //  �¶���ֵ
int TH_Hum = 50;   // ʪ����ֵ
int TH_Gas = 80;   // ������ֵ
int TH_light = 30;   // ����ǿ����ֵ

uint8_t Led_position = 0;         // led�ĵ�λ 
uint8_t machinery_position = 0;    // �����λ

uint8_t tem_flag = 0;     // �¶ȱ�־λ
uint8_t Hum_flag = 0;	  // ʪ�ȱ�־��
uint8_t Gas_flag = 0;	  // ������־λ
uint8_t light_flag = 0;    // ���ձ�־λ
// ==============================����������ֵ end================================


// ============================ user start ==============================
uint8_t beep_flag = 0;    // 1 warning1   2 Error1

// ============================ user end ==============================




