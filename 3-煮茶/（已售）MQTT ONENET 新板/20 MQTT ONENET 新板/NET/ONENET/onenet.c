/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*
	*	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "mqttkit.h"

//Ӳ������
#include "usart.h"
#include "delay.h"
//#include "sht20.h"

//C��
#include <string.h>
#include <stdio.h>

//json��
#include"cjson.h"


#include"led.h"

#define PROID		"594544"//��ƷID

#define AUTH_INFO	"123456"//��Ȩ��Ϣ���Զ��壩

#define DEVID		"1071716757"//�豸ID

u8 flag;
extern unsigned char esp8266_buf[128];
extern u8 humidityH;	  //ʪ����������
extern u8 humidityL;	  //ʪ��С������
extern u8 temperatureH;   //�¶���������
extern u8 temperatureL;   //�¶�С������

 u8 LED0_FLAG;
 u8 LED1_FLAG;
//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	unsigned char *dataPtr;
	
	_Bool status = 1;
	//��ӡһ����Ϣ��Ʒid,��Ȩ��Ϣ���豸ID
   printf("OneNet_DevLink\r\nPROID: %s,	AUIF: %s,	DEVID:%s\r\n", PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP8266_GetIPD(250);									//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)//���ƽ̨�������ݲ�Ϊ����
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)//	MQTT���ݽ��������жϣ�connack���ģ�
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))//��ӡ�Ƿ����ӳɹ�������ʧ�ܵ�ԭ��
				{
					case 0:printf( "Tips:	���ӳɹ�\r\n");status = 0;break;
					
					case 1:printf(  "WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:printf(  "WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
					case 3:printf(  "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:printf(  "WARN:	����ʧ�ܣ��û������������\r\n");break;
					case 5:printf(  "WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
					
					default:printf(  "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
		printf( "WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}

unsigned char OneNet_FillBuf(char *buf)
{
	
	char text[32];
	
	LED0_FLAG=GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);//��ȡLED�Ŀ���״̬������Ӧ���ŵģ�
	LED1_FLAG=GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5);
	
	printf("LED0_FLAG_TYPE=%d\n",sizeof(LED0_FLAG));
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, ",;");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "Tempreture,%d.%d;",temperatureH,temperatureL);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "Humidity,%d.%d;", humidityH,humidityL);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "LED0,%d;", LED0_FLAG);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "LED1,%d;", LED1_FLAG);
	strcat(buf, text);
	printf("buf_mqtt=%s\r\n",buf);
	return strlen(buf);

}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[128];
	
	short body_len = 0, i = 0;
	
	printf( "Tips:	OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));//�����������
	
	body_len = OneNet_FillBuf(buf);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0)							//���
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
			printf( "Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
		}
		else
			printf(  "WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short req_len = 0;
  unsigned char type = 0;

	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;

	
	cJSON *json , *json_value;
  cJSON *json1, *json_value1;


	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//�����·�
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
			if(result == 0)
			{
				//��ӡ�յ�����Ϣ
				printf(  "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				// �����ݰ�req_payload����JSON��ʽ����
				json = cJSON_Parse(req_payload);
				
				if (!json)//���json����Ϊ�գ����ӡ������Ϣ
					printf("Error before: [%s]\n",cJSON_GetErrorPtr());
				else
				{
					json_value = cJSON_GetObjectItem(json , "LED0");//��ȡ��Ӧ���Ե���ֵ
  				//	printf("json_value: [%s]\r\n",json_value->string);//ת��Ϊ�ַ�����ֵ
  				//	printf("json_value: [%d]\r\n",json_value->valueint);//ת��Ϊ��ֵ����ֵ
					
					if((json_value->valueint)==1)
						//flag=1;//�ص�
						GPIO_SetBits(GPIOA,GPIO_Pin_1);						 //PA.A �����
					
					else if((json_value->valueint)==0)					
						//flag=2;//����
					GPIO_ResetBits(GPIOA,GPIO_Pin_1);

							
					
				}
				//ͬ��
				json1 = cJSON_Parse(req_payload);
				if (!json1)
					printf("Error before: [%s]\n",cJSON_GetErrorPtr());
				else
				{
 					json_value1 = cJSON_GetObjectItem(json1 , "LED1");

					if((json_value1->valueint)==1)//����ֵ
						//flag=3;//�ص�
					LED1=1;
					else if((json_value1->valueint)==0)
						//flag=4;//����
					LED1=0;
				
				}
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//����ظ����
				{
					printf( "Tips:	Send CmdResp\r\n");
					
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ظ�����
					MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
				}
				cJSON_Delete(json);//�ͷ�λ�ڶ���cJSON�ṹ���ڴ�
				cJSON_Delete(json1);
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				printf(  "Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//��ջ���
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//����'}'

	if(dataPtr != NULL && result != -1)					//����ҵ���
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;
		
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
	}

	
	
	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
