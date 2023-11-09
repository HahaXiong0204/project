//��Ƭ��ͷ�ļ�
#include "main.h"

//�����豸
#include "wifi.h"

//Э���ļ�
#include "onenet.h"
#include "mqttkit.h"

//Ӳ������
#include "usart.h"



//C��
#include <string.h>
#include <stdio.h>

//CJSON��
#include "cJSON.h"

#include "delay.h"
//#include "24cxx.h"





extern const char *topics[];      // ���ĵ�����





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
	
	printf( "OneNet_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
                        , PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP8266_GetIPD(250);									//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:printf( "Tips:	���ӳɹ�\r\n");status = 0;break;
					
					case 1:printf( "WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:printf( "WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
					case 3:printf( "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:printf( "WARN:	����ʧ�ܣ��û������������\r\n");break;
					case 5:printf( "WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
					
					default:printf( "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
		HAL_Delay(500);
		OneNet_Subscribe(topics, 1);   // ��������
		HAL_Delay(1000);
		OneNet_Send_Device_ID_Data();
		HAL_Delay(1000);
		
	}
	else
		printf( "WARN:	MQTT_PacketConnect Failed\r\n");
	
    

	return status;
	
}


// 1 ���û�
void OneNet_Send_Rfid(uint8_t mode)
{
	cJSON *send_cjson = NULL;
    char *str = (char *)my_malloc(1024);
    send_cjson = cJSON_CreateObject();   // ����cjson
    if(send_cjson != NULL)
    {
		cJSON_AddStringToObject(send_cjson,"Device_ID",PROID);
		if(mode == 1)
		{
			cJSON_AddNumberToObject(send_cjson,"project_num",select);	//��Ӹ���������
		}
		else if(mode == 3)
		{
			cJSON_AddNumberToObject(send_cjson,"settlement",settlement);	//��Ӹ���������
		}
		else{
			cJSON_AddNumberToObject(send_cjson,"project_num",select);	//��Ӹ���������
		}
		
		cJSON_AddNumberToObject(send_cjson,"ID_Cart",ID_Cart);	//��Ӹ���������
		str = cJSON_PrintUnformatted(send_cjson);
		cJSON_Delete(send_cjson);		// �ͷ��ڴ�
		Send_queue(str);
        my_free(str);
	}
}


void OneNet_Send_yishi_Rfid(void)
{
	cJSON *send_cjson = NULL;
    char *str = (char *)my_malloc(1024);
    send_cjson = cJSON_CreateObject();   // ����cjson
    if(send_cjson != NULL)
    {
		cJSON_AddStringToObject(send_cjson,"Device_ID",PROID);
		cJSON_AddNumberToObject(send_cjson,"ID_Cart",yishi);	//��Ӹ���������
		cJSON_AddNumberToObject(send_cjson,"yishi_flag",1);	//��Ӹ���������
		str = cJSON_PrintUnformatted(send_cjson);
		cJSON_Delete(send_cjson);		// �ͷ��ڴ�
		Send_queue(str);
        my_free(str);
	}
}


// �����ϴ���װ����  mode:0 ����   mode:1 �ط�
int id =0;
uint8_t Full_Json(char *buf,uint8_t mode)
{
	cJSON *send_cjson = NULL;
	char *str = (char*)my_malloc(512);
	send_cjson = cJSON_CreateObject();   // ����cjson
	//	cJSON_AddStringToObject(send_cjson,"name","MQ");		//����ַ��� 
//	cJSON_AddNumberToObject(send_cjson,"age",25);	    	//����������� 
//	cJSON_AddNumberToObject(send_cjson,"height",183.52);	//��Ӹ���������
//	cJSON_AddFalseToObject(send_cjson,"gender");			//����߼�ֵfalse
	
	if(mode == 1)
	{
		cJSON_AddNumberToObject(send_cjson,"Again_flag",1);		//����ַ��� 
		
	}
	cJSON_AddStringToObject(send_cjson,"Device_ID",PROID);
	cJSON_AddNumberToObject(send_cjson,"temperature",temperature);	//��Ӹ���������
	cJSON_AddNumberToObject(send_cjson,"humidity",humidity);	//��Ӹ���������
	cJSON_AddNumberToObject(send_cjson,"Lighting",Lighting);	//��Ӹ���������
	cJSON_AddNumberToObject(send_cjson,"Voltage",voltage);	//��Ӹ���������
	cJSON_AddNumberToObject(send_cjson,"Turbidity",Turbidity);	//��Ӹ���������
	cJSON_AddNumberToObject(send_cjson,"Gas",Gas);	//��Ӹ���������
	cJSON_AddNumberToObject(send_cjson,"fire_flag",fire_flag);	//��Ӹ���������
	str = cJSON_PrintUnformatted(send_cjson);
	strcpy(buf,str);
	cJSON_Delete(send_cjson);
	my_free(str);
	return strlen(buf);
}


void OneNet_SendData_Buff(char * buf)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���
	uint16_t body_len = strlen(buf);
	OneNet_Publish("three",buf);
	osDelay(500);
	Send_queue(buf);
//	Wait_send_mutex();
//	uint16_t i = 0;
//	if(body_len)
//	{
//		
//		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 3, &mqttPacket) == 0)							//�����$dp
//		{

//			for(i=0; i < body_len; i++){
//				mqttPacket._data[mqttPacket._len++] = buf[i];
//			}
//			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
//			printf( "Send %d Bytes\r\n", mqttPacket._len);
//			
//			MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
//		}
//		else
//			printf( "WARN:	EDP_NewBuffer Failed\r\n");
//		
//		
//       
//	}
//	Release_send_mutex();
}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//			mode:0 ����   mode:1 �ط�
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(uint8_t mode)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[256];
	
	short body_len = 0, i = 0;
	
	printf( "Tips:	OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = Full_Json(buf,mode);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
	Send_queue(buf);
//	Wait_send_mutex();
//	if(body_len)
//	{
//		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 3, &mqttPacket) == 0)							//�����$dp
//		{
//			for(i=0; i < body_len; i++){
//				mqttPacket._data[mqttPacket._len++] = buf[i];
//			}
//			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
//			printf( "Send %d Bytes\r\n", mqttPacket._len);
//			
//			MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
//		}
//		else
//			printf( "WARN:	EDP_NewBuffer Failed\r\n");
//       
//	}

//	Release_send_mutex();
}


void OneNet_Send_Device_ID_Data(void)
{
	cJSON *send_cjson = NULL;
    char *str = (char *)my_malloc(1024);
    send_cjson = cJSON_CreateObject();   // ����cjson
    if(send_cjson != NULL)
    {
        cJSON_AddNumberToObject(send_cjson, "state", 0);			//���״ֵ̬
		cJSON_AddStringToObject(send_cjson,"Device_ID",PROID);
        str = cJSON_PrintUnformatted(send_cjson);
        cJSON_Delete(send_cjson);		// �ͷ��ڴ�
		Send_queue(str);
        my_free(str);

    }

}

void OneNet_Send_Int(uint8_t *key ,uint16_t value)
{
	cJSON *send_cjson = NULL;
    char *str = (char *)my_malloc(1024);
    send_cjson = cJSON_CreateObject();   // ����cjson
    if(send_cjson != NULL)
    {
		cJSON_AddNumberToObject(send_cjson,(char*)key,value);
		str = cJSON_PrintUnformatted(send_cjson);
		cJSON_Delete(send_cjson);		// �ͷ��ڴ�
		Send_queue(str);
        my_free(str);
	}
}

void OneNet_Send_String(uint8_t *key ,uint8_t *value)
{
	cJSON *send_cjson = NULL;
    char *str = (char *)my_malloc(1024);
    send_cjson = cJSON_CreateObject();   // ����cjson
    if(send_cjson != NULL)
    {
		cJSON_AddStringToObject(send_cjson,(char*)key,(char*)value);
		str = cJSON_PrintUnformatted(send_cjson);
		cJSON_Delete(send_cjson);		// �ͷ��ڴ�
		Send_queue(str);
        my_free(str);
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
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	uint8_t pianyi = 0;   // dataPtr��ƫ����
	char numBuf[10];
	int num = 0;
	
	char *buff_time;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//�����·�
		{
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
			if(result == 0)
			{
				printf( "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//����ظ����
				{
					printf( "Tips:	Send CmdResp\r\n");
					
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ظ�����
					MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
				}
			}
		
		break;
		}
		case MQTT_PKT_PUBLISH:														//���յ�Publish��Ϣ
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				printf( "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
				
				cJSON *cjson = NULL;
				
				//���Բ���
				double temp_value,humi_value;
				
				//JSON�ַ�����cJSON��ʽ
				cjson = cJSON_Parse(req_payload); 
				//�ж�cJSON_Parse��������ֵȷ���Ƿ����ɹ�
				if(cjson == NULL){
					  printf( "json pack into cjson error...\r\n");
				}
                // ��������
				else{
                    
					//��ȡ�ֶ�ֵ
					//cJSON_GetObjectltem���ص���һ��cJSON�ṹ���������ǿ���ͨ���������ؽṹ��ķ�ʽѡ�񷵻����ͣ�
//					OneNet_Send_Int("ack",1);
					uint8_t flag = 0;
					printf("cjson �ڴ��СΪ = %d\r\n",sizeof(cjson));
					
	// ===========================================�������յ�������=====================================================
					if(cJSON_GetObjectItem(cjson,"temperature")!=NULL)
					{
						temp_value = cJSON_GetObjectItem(cjson,"temperature")->valuedouble;
						printf("csjon�����ɹ� temp_value = %f\r\n",temp_value);
					}
					
					if(cJSON_GetObjectItem(cjson,"flag")!=NULL)
					{
						flag = cJSON_GetObjectItem(cjson,"flag")->valueint;
						printf("csjon�����ɹ� flag = %d\r\n",flag);
					}
					
					if(cJSON_GetObjectItem(cjson,"Humidity")!=NULL)
					{
						humi_value = cJSON_GetObjectItem(cjson,"Humidity")->valuedouble;
						printf("csjon�����ɹ� Humidity = %f\r\n",humi_value);
					}
					
					// �û�ģʽ
					if(cJSON_GetObjectItem(cjson,"user_mode")!=NULL)
					{
						user_mode = cJSON_GetObjectItem(cjson,"user_mode")->valuedouble;
						printf("csjon�����ɹ� user_mode = %d\r\n",user_mode);
					}
					
					
					if(cJSON_GetObjectItem(cjson,"TH_Temp")!=NULL)
					{
						TH_Temp = cJSON_GetObjectItem(cjson,"TH_Temp")->valueint;
						printf("csjon�����ɹ� TH_Temp = %d\r\n",TH_Temp);
					}
					
					if(cJSON_GetObjectItem(cjson,"TH_Hum")!=NULL)
					{
						TH_Hum = cJSON_GetObjectItem(cjson,"TH_Hum")->valueint;
						printf("csjon�����ɹ� TH_Temp = %d\r\n",TH_Hum);
					}
					
					if(cJSON_GetObjectItem(cjson,"TH_light")!=NULL)
					{
						TH_light = cJSON_GetObjectItem(cjson,"TH_light")->valueint;
						printf("csjon�����ɹ� TH_light = %d\r\n",TH_light);
					}
					
					
					if(cJSON_GetObjectItem(cjson,"TH_Gas")!=NULL)
					{
						TH_Gas = cJSON_GetObjectItem(cjson,"TH_Gas")->valueint;
						printf("csjon�����ɹ� TH_Gas = %d\r\n",TH_Gas);
					}
					
					
					if(cJSON_GetObjectItem(cjson,"Led_position")!=NULL)
					{
						Led_position = cJSON_GetObjectItem(cjson,"Led_position")->valueint;
						printf("csjon�����ɹ� Led_position = %d\r\n",Led_position);
					}
					
					if(cJSON_GetObjectItem(cjson,"machinery_position")!=NULL)
					{
						machinery_position = cJSON_GetObjectItem(cjson,"machinery_position")->valueint;
						printf("csjon�����ɹ� machinery_position = %d\r\n",machinery_position);
					}
					
					
					/*
						extern uint16_t year_time;
						extern uint8_t moon_time;
						extern uint8_t sun_time;
						extern uint8_t hour_time;   // Сʱ
						extern uint8_t minute_time;  // ����
						extern uint8_t second_time;  // ��
					*/
					if(cJSON_GetObjectItem(cjson,"time")!=NULL)
					{
						buff_time = cJSON_GetObjectItem(cjson,"time")->valuestring;
						// 23-3-4-4-4-
						sscanf(buff_time,"%d-%d-%d-%d-%d-%d",&year_time,&moon_time,&sun_time,&hour_time,&minute_time,&second_time);
						time_init(year_time,moon_time,sun_time,hour_time,minute_time,second_time);
						state = 1;  
						printf("time csjon�����ɹ�\r\n");
					}
					
					
					
					

					//======================================================================================

				}
				 
					//delete cjson

				cJSON_Delete(cjson);
				printf("------------------------------------�ͷųɹ�-----------------------------------------\r\n");
				
				switch(qos)
				{
					case 1:															//�յ�publish��qosΪ1���豸��Ҫ�ظ�Ack
					
						if(MQTT_PacketPublishAck(pkt_id, &mqttPacket) == 0)
						{
							printf( "Tips:	Send PublishAck\r\n");
							ESP8266_SendData(mqttPacket._data, mqttPacket._len);
							MQTT_DeleteBuffer(&mqttPacket);
						}
					
					break;
					
					case 2:															//�յ�publish��qosΪ2���豸�Ȼظ�Rec
																					//ƽ̨�ظ�Rel���豸�ٻظ�Comp
						if(MQTT_PacketPublishRec(pkt_id, &mqttPacket) == 0)
						{
							printf( "Tips:	Send PublishRec\r\n");
							ESP8266_SendData(mqttPacket._data, mqttPacket._len);
							MQTT_DeleteBuffer(&mqttPacket);
						}
					
					break;
					
					default:
						break;
				}
                ESP8266_Clear();									//��ջ���
				if(dataPtr!=NULL)
				{
					my_free(dataPtr);
					dataPtr = NULL;
					printf("�ͷ�dataptr�ռ�ɹ�\r\n");
				}
				if(req_payload!=NULL)
				{
					my_free(req_payload);
					req_payload = NULL;
				}
				if(cmdid_topic!=NULL)
				{
					my_free(cmdid_topic);
					cmdid_topic =NULL;
				}
                return ;
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				printf( "Tips:	MQTT Publish Send OK\r\n");
			
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
		pianyi++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
			pianyi++;
		}
		numBuf[num] = 0;
		printf("09 -============\r\n");
        
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ

//        //=================$crep���յĴ���start================
        result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
        
        printf( "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);


        
        //=================$crep���մ���end=====================
        
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
	dataPtr -= pianyi;
	
		
		
}

//==========================================================
//	�������ƣ�	OneNet_Publish
//
//	�������ܣ�	������Ϣ
//
//	��ڲ�����	topic������������
//				msg����Ϣ����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_PUBLISH-��Ҫ����
//
//	˵����		
//==========================================================
void OneNet_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	/*���ط�0��ʾδ�ܸ�mqttPacketЭ�����ȫ�������*/
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
        printf("Publish Topic: %s, Msg: %s\r\n", topic, msg);

		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}
    else{
    	printf("Publish Error!\r\n");
    }

}



//==========================================================
//	�������ƣ�	OneNet_Subscribe
//
//	�������ܣ�	����
//
//	��ڲ�����	topics�����ĵ�topic
//				topic_cnt��topic����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_SUBSCRIBE-��Ҫ�ط�
//
//	˵����		
//==========================================================
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	for(; i < topic_cnt; i++)
		printf( "Subscribe Topic: %s\r\n", topics[i]);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL2, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}

}

