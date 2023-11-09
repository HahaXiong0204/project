#ifndef _ONENET_H_
#define _ONENET_H_

#include "main.h"

#define DEVID		"588884"   //��ƷID

#define AUTH_INFO	"123456"		//��Ȩ��Ϣ

#define PROID	"2001"	//�豸ID


void OneNet_Send_Rfid(uint8_t mode);
_Bool OneNet_DevLink(void);

uint8_t Full_Json(char *buf,uint8_t mode);

void OneNet_Send_Device_ID_Data(void);

void OneNet_Publish(const char *topic, const char *msg);

void OneNet_SendData_Buff(char * buf);

void OneNet_SendData(uint8_t mode);

void OneNet_Send_Int(uint8_t *key ,uint16_t value);
void OneNet_Send_String(uint8_t *key ,uint8_t *value);

void OneNet_RevPro(unsigned char *cmd);

void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt);


#endif
