#include "public.h"


uint8_t i;

// ======================= �̶߳��� ===============================================
QueueHandle_t hsend_queue;    // ���͵Ķ���
char *send_buffer = NULL;    // ���͵���������buff



QueueHandle_t hreve_queue;    // ���յĶ���
BaseType_t hreve_queue_base = pdFALSE;   // ���յĺ�


TaskHandle_t  send_data_id;    // ���������߳�
TaskHandle_t  reve_data_id;    // ���������߳�

TaskHandle_t  show_lcd_id;   	// ��Ļ��ʾ
TaskHandle_t  collect_data_id;   // ���ݲɼ��߳�
TaskHandle_t  read_rfid_id;   // ��ȡrfid�߳�
TaskHandle_t  Logical_code_id;   // �߼��ж��߳�
TaskHandle_t  key_scan_id;   // ����ɨ���߳�


SemaphoreHandle_t send_mutex;   // ������
SemaphoreHandle_t reve_mutex;   // ������
SemaphoreHandle_t iic_spi_mutex;   // ����iic��spi����

SemaphoreHandle_t send_sem = NULL;

// ===============================================================================

// =========================== wifi ==========================
uint8_t cmd_flag = 0;    // �����ӡ��־λ
uint8_t net_connect = 0;            // �����Ƿ����ӵı�־λ
uint8_t rx_buff[1024];
uint16_t rx_cnt = 0;
uint8_t again_num = 0;

uint8_t again_mode = 0;   // �ط���־��
uint8_t state = 0;    // �Ƿ�λ
uint8_t user_uing = 0;    // �û�ʹ�ñ�־λ
uint8_t user_mode = 0;   // 0 �Զ�  1 �ֶ�

uint8_t reconnect_num = 0;    // ��������
uint8_t reconnect_one_num = 0;   // ������������
// ============================================================




// ========================== ���������� start ==========================
uint32_t adc_value[ADC_MAX];              //adc���ݻ�����
double temperature = 0;             //�¶�
double temperature_ds18 = 0;             //�¶�
double humidity = 0;                //ʪ��
uint32_t Lighting = 0, Turbidity = 0, Gas = 0; //���գ��Ƕ�
double voltage = 0;  // ��ѹ
uint16_t speed = 0;     // ����ٶ�
uint8_t people = 0;   // ���崫����  0����  1����
uint8_t red_people = 0;   // ���崫����  0����  1����
uint8_t Led_position = 0;         // led�ĵ�λ
uint8_t machinery_position = 0;    // �����λ  
uint8_t approch_switch = 0;  // �ӽ����� 1 û������0������


uint8_t tem_flag = 0;     // �¶ȱ�־λ
uint8_t Hum_flag = 0;	  // ʪ�ȱ�־��
uint8_t Gas_flag = 0;	  // ������־λ
uint8_t light_flag = 0;    // ���ձ�־λ
uint8_t fire_flag = 0;   // �����־λ
uint8_t approch_flag = 0;    // �ӽ����صı�־λ
// ===========================���������� end =====================================




// ============================= ����������ֵ start====================
int TH_Temp = 48;   //  �¶���ֵ
int TH_Hum = 60;   // ʪ����ֵ
int TH_Gas = 99;   // ������ֵ
int TH_light = 30;   // ����ǿ����ֵ
// ==============================����������ֵ end================================


// =========================== ϵͳ ===============================================
char time_buff[56];
char start_time_buff[56];   // ��ʼ��ˮʱ��
// ==============================================================================

// ============================ user start ==============================
uint8_t beep_flag = 0;    // 1 warning1   2 Error1


uint16_t Water_h = 100; // ��ˮλ
uint16_t Water_l = 20; // ��ˮλ
uint16_t Water_c = 50; // ��ǰˮλ

uint16_t Water_TDS1 = 0;   // ˮԴˮ��
uint16_t Water_TDS2 = 0;   // ��ˮˮ��

int TL_Temp = 50;    // ����¶�

uint8_t Error_flag = 0;   // �����־λ
uint8_t Warning_flag = 0;  // �����־λ

float Water_Hot = 0;   // ��ˮ�¶�
float Water_Hot_temp = 0;   // ģ����ˮ�¶�
uint8_t Water_flow = 10;   // ��λʱ��ˮ����

uint8_t Hot_flag = 0;  // ���ȱ�־λ

uint8_t Water_switch = 0;    // ��ˮ

uint8_t Water_leve_flag = 0;  // ��ˮˮƽ 0 ���� 1��ˮ

uint16_t Water_all = 0;    // ��ˮ����
uint16_t Water_one_all = 0;   // ������ˮ����

uint8_t jiesuan_flag = 0;   // �����־λ



// ============================ user end ==============================




