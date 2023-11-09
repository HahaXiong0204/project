#ifndef _WIFI_H_
#define _WIFI_H_
#include "main.h"
#define MAX_SIZE 1024


void ESP8266_Clear(void);
_Bool ESP8266_SendCmd(char *cmd, char *res);
void ESP8266_Init(void);
unsigned char *ESP8266_GetIPD(unsigned short timeOut);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
void ESP8266_SendData(unsigned char *data, unsigned short len);
#endif

