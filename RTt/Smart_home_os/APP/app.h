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


void Mx_Init(void);  // 初始化数据

void lcd_show(void *pargem); // 显示数据在lcd屏幕上
void collect_data(void *pargem);   // 采集传感器数据

void Logical_code_entry(void *pargem);   // 逻辑判断
void at_sava_th(void);    // 保存阈值数据
void read_rfid(void *pargem);   // 读rfid

void resest_wifi(void);      // 断网重连wifi
void resest_server(void);   // 断网重连服务器


// 设置LED档位
void Led_pwm_mode(uint8_t mode);
// 设置电机档位
void machinery_pwm_mode(uint8_t mode);






#endif
