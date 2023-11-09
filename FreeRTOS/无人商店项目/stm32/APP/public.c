#include "public.h"


uint8_t i;

// ======================= 线程定义 ===============================================
QueueHandle_t hsend_queue;    // 发送的队列
char *send_buffer = NULL;    // 发送到缓冲区的buff



QueueHandle_t hreve_queue;    // 接收的队列
BaseType_t hreve_queue_base = pdFALSE;   // 接收的宏


TaskHandle_t  send_data_id;    // 发送数据线程
TaskHandle_t  reve_data_id;    // 接收数据线程

TaskHandle_t  show_lcd_id;   	// 屏幕显示
TaskHandle_t  collect_data_id;   // 数据采集线程
TaskHandle_t  read_rfid_id;   // 读取rfid线程
TaskHandle_t  Logical_code_id;   // 逻辑判断线程
TaskHandle_t  key_scan_id;   // 按键扫描线程


SemaphoreHandle_t send_mutex;   // 发送锁
SemaphoreHandle_t reve_mutex;   // 接收锁
SemaphoreHandle_t iic_spi_mutex;   // 保护iic和spi的锁

SemaphoreHandle_t send_sem = NULL;

// ===============================================================================

// =========================== wifi ==========================
uint8_t cmd_flag = 0;    // 命令打印标志位
uint8_t net_connect = 0;            // 网咯是否连接的标志位
uint8_t rx_buff[1024];
uint16_t rx_cnt = 0;
uint8_t again_num = 0;

uint8_t again_mode = 0;   // 重发标志问
uint8_t state = 0;    // 是否复位
uint8_t user_uing = 0;    // 用户使用标志位
uint8_t user_mode = 0;   // 0 自动  1 手动

uint8_t reconnect_num = 0;    // 重连次数
uint8_t reconnect_one_num = 0;   // 单次重连次数
// ============================================================




// ========================== 传感器数据 start ==========================
uint32_t adc_value[ADC_MAX];              //adc数据缓冲区
double temperature = 0;             //温度
double humidity = 0;                //湿度
uint32_t Lighting = 0, Turbidity = 0, Gas = 0; //光照，浊度
double voltage = 0;  // 电压
uint16_t speed = 0;     // 电机速度
uint8_t people = 0;   // 人体传感器  0无人  1有人
uint8_t Led_position = 0;         // led的档位
uint8_t machinery_position = 0;    // 电机档位  



uint8_t tem_flag = 0;     // 温度标志位
uint8_t Hum_flag = 0;	  // 湿度标志物
uint8_t Gas_flag = 0;	  // 空气标志位
uint8_t light_flag = 0;    // 光照标志位

// ===========================传感器数据 end =====================================




// ============================= 传感器的阈值 start====================
int TH_Temp = 48;   //  温度阈值
int TH_Hum = 60;   // 湿度阈值
int TH_Gas = 99;   // 空气阈值
int TH_light = 30;   // 光照强度阈值
// ==============================传感器的阈值 end================================


// ============================ user start ==============================
uint8_t beep_flag = 0;    // 1 warning1   2 Error1

int Water_num = 10;   // 矿泉水 数量
float Water_p = 2.0;   // 饮水机价格

int bingan_num = 10;   // 饼干数量
float bingan_p = 4.5;  // 饼干价格

int yinlian_num = 10;  // 饮料 数量
float yinliao_p = 5.0;  // 饮料价格


int Water_g = 0;   // 矿泉水购买量
int bingan_g = 0;  // 饼干购买量
int yinliao_g = 0; // 饮料购买量

uint8_t stor_flag = 1; //  商品标号

float calu_num = 0;    // 计算总价
uint8_t nua = 0;

uint8_t people_flag =0 ;
// ============================ user end ==============================




