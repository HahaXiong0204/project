#ifndef _PUBLIC_H
#define _PUBLIC_H
#include "cmsis_os.h"
#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include "queue.h"
#include "sys.h"
#include "wifi.h"
#include "string.h"
#include "tim.h"
#include "onenet.h"
#include "thread.h"
#include "hc-sr04.h"
#include "semphr.h"
#include "iwdg.h"
#include "cjson.h"
#include "duoji.h"
#include "ds18b20.h"
#include "DS18B20_2.h"



#define AT_24_DATA_MAX  100   // �������
#define AT_24_Init_ADRR	 4     // ��ʼ�ĵ�ַ

struct AT24CDATA
{
    uint8_t 	arr[AT_24_DATA_MAX];
    uint16_t 	len;
    uint16_t	addr;
};


extern uint16_t  ID_Cart;
extern uint8_t i;


// =========================== ϵͳ ===============================================
extern char time_buff[56];     // ƴװʱ��

// ==============================================================================



// ======================= �̶߳��� ===============================================
#define SEND_MAX_QUEUE		10  // �����з���
extern QueueHandle_t hsend_queue;    // ���͵Ķ���
extern char *send_buffer;    // ���͵���������buff


#define REVE_MAX_QUEUE		10  // �����н���
extern QueueHandle_t hreve_queue;    // ���յĶ���
extern BaseType_t hreve_queue_base;   // ���յĺ�

extern TaskHandle_t  send_data_id;    // ���������߳�
extern TaskHandle_t  reve_data_id;    // ���������߳�

extern TaskHandle_t  show_lcd_id;   	// ��Ļ��ʾ
extern TaskHandle_t  collect_data_id;   // ���ݲɼ��߳�
extern TaskHandle_t  read_rfid_id;   // ��ȡrfid�߳�
extern TaskHandle_t  Logical_code_id;   // �߼��ж��߳�
extern TaskHandle_t  key_scan_id;   // ����ɨ���߳�

extern SemaphoreHandle_t send_mutex;   // ������
extern SemaphoreHandle_t reve_mutex;   // ������
extern SemaphoreHandle_t iic_spi_mutex;   // ����iic��spi����
// ===============================================================================

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
extern uint8_t state; 		   // �Ƿ�λ
extern uint8_t reconnect_num;    // ��������
extern uint8_t reconnect_one_num;   // ������������
// ============================================================



// ========================== ���������� ==========================
#define ADC_MAX 	40
extern uint32_t adc_value[ADC_MAX];			//adc���ݻ�����
extern double temperature;               //�¶�
extern double temperature_ds18;             //�¶�
extern double humidity;                  //ʪ��
extern uint32_t Lighting, Turbidity, Gas; //���գ��Ƕȣ����������
extern double voltage;    // ��ѹ
extern uint16_t speed;     // ����ٶ�
extern uint8_t people;   // ���崫����
extern uint8_t red_people;   // ���崫����  0����  1����
extern uint8_t Led_position;         // led�ĵ�λ
extern uint8_t machinery_position;    // �����λ

extern uint8_t tem_flag;     // �¶ȱ�־λ
extern uint8_t Hum_flag;	  // ʪ�ȱ�־��
extern uint8_t Gas_flag;	  // ������־λ
extern uint8_t light_flag;    // ���ձ�־λ
extern uint8_t fire_flag;   // �����־λ
extern uint8_t approch_switch;  // �ӽ����� 1 û������0������
extern uint8_t approch_flag;    // �ӽ����صı�־λ

extern int TH_Temp;   //  �¶���ֵ
extern int TL_Temp;    // ����¶�
// ================================================================

extern float Water_Hot_temp;   // ģ����ˮ�¶�
// ============================= ����������ֵ ====================
extern int TH_Temp;   //  �¶���ֵ
extern int TH_Hum;   // ʪ����ֵ
extern int TH_Gas;   // ������ֵ
extern int TH_light;   // ����ǿ����ֵ
// ==============================================================


// ============================ user start ==============================
extern uint8_t beep_flag;    // 1 warning1   2 Error1    1->0.1s�졢0.9����    2->0.5s
extern  uint8_t user_uing;    // �û�ʹ�ñ�־λ


extern uint16_t Water_h; // ��ˮλ
extern uint16_t Water_l; // ��ˮλ
extern uint16_t Water_c ; // ��ǰˮλ

extern uint16_t Water_TDS1;   // ˮԴˮ��
extern uint16_t Water_TDS2;   // ��ˮˮ��

extern uint8_t Error_flag;   // �����־λ
extern uint8_t Warning_flag;  // �����־λ

extern float Water_Hot;   // ��ˮ�¶�

extern uint8_t Water_flow;   // ��λʱ��ˮ����
extern uint8_t Hot_flag;  // ���ȱ�־λ

extern uint8_t Water_switch;    // ��ˮ

extern uint8_t Water_leve_flag;  // ��ˮˮƽ 0 ���� 1��ˮ



#define WATER_ONE_MAX   120
extern uint16_t Water_all;    // ��ˮ����
extern uint16_t Water_one_all;   // ������ˮ����

extern uint8_t jiesuan_flag;   // �����־λ
// ============================ user end ==============================

// =========================== ϵͳ ===============================================
extern char time_buff[56];     // ƴװʱ��
extern uint8_t *PROID_Device; 
extern char start_time_buff[56];   // ��ʼ��ˮʱ��
// ==============================================================================


#endif
