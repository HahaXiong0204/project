#ifndef _ONENET_H_
#define _ONENET_H_

#include "main.h"

_Bool OneNet_DevLink(void);

uint8_t Full_Json(char *buf,uint8_t mode);

void OneNet_Send_Device_ID_Data(void);

void OneNet_Publish(const char *topic, const char *msg);

void OneNet_SendData_Buff(char * buf);

void OneNet_SendData(uint8_t mode);

void OneNet_RevPro(unsigned char *cmd);

void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt);


#endif
