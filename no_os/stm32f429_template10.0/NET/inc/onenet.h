#ifndef _ONENET_H_
#define _ONENET_H_


#include "main.h"
#include "delay.h"

extern const char *topics[];      // ¶©ÔÄµÄÖ÷Ìâ

_Bool OneNet_DevLink(void);

//uint8_t Full_Json(char *buf);
uint16_t OneNet_FillBuf(char *buf,uint8_t again_flag);
void OneNet_SendData_State(char *buf);
uint8_t OneNet_SendData_Buff(char * buf);

void OneNet_SendData(void);

void OneNet_RevPro(unsigned char *cmd);

void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt);


#endif
