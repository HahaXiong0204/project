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
void Mx_Init(void);  // 初始化数据

void lcd_show(void); // 显示数据在lcd屏幕上
void collect_data(uint8_t mode);   // 采集传感器数据

void at_sava_th(void);
void resest_wifi(void);      // 断网重连wifi
void resest_server(void);   // 断网重连服务器










#endif
