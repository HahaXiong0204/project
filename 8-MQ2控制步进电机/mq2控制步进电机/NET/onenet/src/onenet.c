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
#include "oled.h"



//C��
#include <string.h>
#include <stdio.h>

//CJSON��
#include "cJSON.h"



#define PROID		"588384"   //��ƷID

#define AUTH_INFO	"123456"		//��Ȩ��Ϣ

#define DEVID		"1064133490"	//�豸ID


extern unsigned char esp8266_buf[128];
extern float Gas;
extern uint8_t flag ;
extern u8 mode ;   // 0 Ϊ�Զ�  1 Ϊ�ֶ�
extern u8 th_Gas ;

extern void motorNcircle(int n,u8 position);

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
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
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
					case 0:UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��û������������\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
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
		UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topics[i]);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL2, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}

}



unsigned char OneNet_FillBuf(char *buf)
{
	
	char text[32];

	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{");
    
    memset(text, 0, sizeof(text));
	sprintf(text, "\"mode\":\"%d\",", mode);
	strcat(buf, text);
    
    memset(text, 0, sizeof(text));
	sprintf(text, "\"flag\":\"%d\",", flag);
	strcat(buf, text);
    
    memset(text, 0, sizeof(text));
	sprintf(text, "\"TH_Gas\":\"%d\",", th_Gas);
	strcat(buf, text);
    
    memset(text, 0, sizeof(text));
	sprintf(text, "\"Gas\":\"%.2f\"", Gas);
	strcat(buf, text);
    

	strcat(buf, "}");
	
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
	
	char buf[256];
	
	short body_len = 0, i = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 3, &mqttPacket) == 0)							//�����$dp
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
			UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
       
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
	char numBuf[10];
	int num = 0;
	
	cJSON* cjson;
	int value;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//�����·�
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
			if(result == 0)
			{
				UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//����ظ����
				{
					UsartPrintf(USART_DEBUG, "Tips:	Send CmdResp\r\n");
					
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ظ�����
					MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
				}
			}
		
		break;
//			
		case MQTT_PKT_PUBLISH:														//���յ�Publish��Ϣ
            printf("��������MQTT_PKT_PUBLISH\r\n");
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				UsartPrintf(USART_DEBUG, "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
				
//				//JSON�ַ�����cJSON��ʽ
				cjson = cJSON_Parse(req_payload); 
				//�ж�cJSON_Parse��������ֵȷ���Ƿ����ɹ�
				if(cjson == NULL){
//						printf("json pack into cjson error...");
					  UsartPrintf(USART_DEBUG, "json pack into cjson error...\r\n");
				}
				else{
					//��ȡ�ֶ�ֵ
				//cJSON_GetObjectltem���ص���һ��cJSON�ṹ���������ǿ���ͨ���������ؽṹ��ķ�ʽѡ�񷵻����ͣ�

                value = 0;  
                value = atoi(cJSON_GetObjectItem(cjson,"mode")->valuestring);
                if(value!=0)
                {
                    mode = value;
                    printf("mode = %d\r\n",mode);
                    PCout(13) = 1;
                }
                
                value = 0;    
				value = atoi(cJSON_GetObjectItem(cjson,"TH_Gas")->valuestring);
                if(value!=0)
                {
                    th_Gas = value;
                    printf("TH_Gas = %d\r\n",th_Gas);
                }
                if(mode == 2)
                {
                    value = 0;    
                    value = atoi(cJSON_GetObjectItem(cjson,"flag")->valuestring);
                    if(value == 1)
                    {
                        PCout(13)=0;    // ����
                        OLED_ShowStr(32,4,":",2); OLED_ShowCN(42,4,66); OLED_ShowCN(58,4,68); // �����ת
                        motorNcircle(1,1);//nΪ�������ת����Ȧ����positionΪ��ת���߷�ת
                        flag = 1;    // ����
                        OLED_ShowStr(32,4,":",2); OLED_ShowCN(42,4,79); OLED_ShowCN(58,4,80); // ���ֹͣ
                    }else if(value == 2){
                        PCout(13) =  1;  // �ص�
                        OLED_ShowStr(32,4,":",2); OLED_ShowCN(42,4,67); OLED_ShowCN(58,4,68); // �����ת
                        motorNcircle(1,0);//nΪ�������ת����Ȧ����positionΪ��ת���߷�ת
                        OLED_ShowStr(32,4,":",2); OLED_ShowCN(42,4,79); OLED_ShowCN(58,4,80); // ���ֹͣ
                        flag = 2;   // ����
                    }
                
                }

                
                
            }
				 
				//delete cjson
				cJSON_Delete(cjson);

				
				
				switch(qos)
				{
					case 1:															//�յ�publish��qosΪ1���豸��Ҫ�ظ�Ack
					
                    
                        
						if(MQTT_PacketPublishAck(pkt_id, &mqttPacket) == 0)
						{
							UsartPrintf(USART_DEBUG, "Tips:	Send PublishAck\r\n");
							ESP8266_SendData(mqttPacket._data, mqttPacket._len);
							MQTT_DeleteBuffer(&mqttPacket);
						}
					
					break;
					
					case 2:															//�յ�publish��qosΪ2���豸�Ȼظ�Rec
																					//ƽ̨�ظ�Rel���豸�ٻظ�Comp
                    

						if(MQTT_PacketPublishRec(pkt_id, &mqttPacket) == 0)
						{
							UsartPrintf(USART_DEBUG, "Tips:	Send PublishRec\r\n");
							ESP8266_SendData(mqttPacket._data, mqttPacket._len);
							MQTT_DeleteBuffer(&mqttPacket);
						}
					
					break;
					
					default:
						break;
				}
                ESP8266_Clear();									//��ջ���
                return ;
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			
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
		printf("09                                                                                                                                           -============\r\n");
        
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ

//        //=================$crep���յĴ���start================
        result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
        
        UsartPrintf(USART_DEBUG, "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);

        //JSON�ַ�����cJSON��ʽ
        cjson = cJSON_Parse(req_payload); 
        // �ж�cJSON_Parse��������ֵȷ���Ƿ����ɹ�
        if(cjson == NULL){
//						printf("json pack into cjson error...");
              UsartPrintf(USART_DEBUG, "json pack into cjson error...\r\n");
        }
        else{
        
            
            value = 0;  
                value = atoi(cJSON_GetObjectItem(cjson,"mode")->valuestring);
                if(value!=0)
                {
                    mode = value;
                    printf("mode = %d\r\n",mode);
                    PCout(13) = 1;
                }
                
                value = 0;    
				value = atoi(cJSON_GetObjectItem(cjson,"TH_Gas")->valuestring);
                if(value!=0)
                {
                    th_Gas = value;
                    printf("TH_Gas = %d\r\n",th_Gas);
                }
                if(mode == 2)
                {
                    value = 0;    
                    value = atoi(cJSON_GetObjectItem(cjson,"flag")->valuestring);
                    if(value == 1)
                    {
                        flag  = 1;
                        motorNcircle(3,1);
                    }else if(value == 2){
                        flag  = 2;
                        motorNcircle(3,0);
                    }
                
                }
            //delete cjson
            cJSON_Delete(cjson);
//            
        }

        
        //=================$crep���մ���end=====================
        
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
