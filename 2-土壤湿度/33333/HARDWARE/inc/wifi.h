
#ifndef __WIFI_H
#define __WIFI_H

#include "usart2.h"	   	

#define RESET_IO(x)    GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)x)  //PE4����WiFi�ĸ�λ

#define WiFi_printf       u2_printf           //����2���� WiFi
#define WiFi_RxCounter    Usart2_RxCounter    //����2���� WiFi
#define WiFi_RX_BUF       Usart2_RxBuff       //����2���� WiFi 
#define WiFi_RXBUFF_SIZE  USART2_RXBUFF_SIZE  //����2���� WiFi

#define SSID   "me_computer"                       //·����SSID����
#define PASS   "2960493425"                       //·��������

#define ESP8266_WIFI_INFO		"AT+CWJAP=\"QXL\",\"123456789\"\r\n"

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"


extern int ServerPort;
extern char ServerIP[128];                    //��ŷ�����IP��������

void WiFi_EnableIO_Init(void);
void WiFi_ResetIO_Init(void);
char WiFi_SendCmd(char *cmd, int timeout);
//char WiFi_SendCmd(char *cmd, char *ret,int timeout);
char WiFi_Reset(int timeout);
char WiFi_JoinAP(int timeout);
char WiFi_Connect_Server(int timeout);
char WiFi_Smartconfig(int timeout);
char WiFi_WaitAP(int timeout);
char WiFi_Connect_IoTServer(void);


void ESP8266_Init(void);

void ESP8266_Clear(void);

#endif


