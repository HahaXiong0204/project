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


uint8_t *PROID_Device = PROID; 


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
//		OneNet_SendData(1);
		
	}
	else
		printf( "WARN:	MQTT_PacketConnect Failed\r\n");
	
    

	return status;
	
}


/*
	��������
mode:
	1 
*/
static char one_buff[256];
void OneNet_Send_one_Data(uint8_t mode)
{
	cJSON *send_cjson = NULL;
	char *str = (char*)my_malloc(512);
	send_cjson = cJSON_CreateObject();   // ����cjson
	memset(one_buff,0,sizeof(one_buff));
	/*
			cJSON_AddStringToObject(send_cjson,"name","MQ");		//����ַ��� 
			cJSON_AddNumberToObject(send_cjson,"age",25);	    	//����������� 
			cJSON_AddNumberToObject(send_cjson,"height",183.52);	//��Ӹ���������
			cJSON_AddFalseToObject(send_cjson,"gender");			//����߼�ֵfalse
	*/
	cJSON_AddStringToObject(send_cjson,"Device_ID",PROID);
	if(mode == 1)
	{
		cJSON_AddNumberToObject(send_cjson,"settlement",1);	//��Ӹ���������
		cJSON_AddNumberToObject(send_cjson,"Water_all",Water_all);	//��Ӹ���������
		Make_Time(time_buff);
		cJSON_AddStringToObject(send_cjson,"time_flag",time_buff);
		cJSON_AddStringToObject(send_cjson,"start_time_buff",start_time_buff);
	}
	str = cJSON_PrintUnformatted(send_cjson);
	strcpy(one_buff,str);
	Send_queue(one_buff);
	cJSON_Delete(send_cjson);
	my_free(str);
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
	
	
		// =============================  ���������� ==============================
		cJSON_AddNumberToObject(send_cjson,"temperature",temperature);	//��Ӹ���������
		cJSON_AddNumberToObject(send_cjson,"hot_temperature",Water_Hot);	//��Ӹ���������
		cJSON_AddNumberToObject(send_cjson,"Turbidity1",Water_TDS1);	//��Ӹ���������
		cJSON_AddNumberToObject(send_cjson,"Turbidity2",Water_TDS2);	//��Ӹ���������
		cJSON_AddNumberToObject(send_cjson,"TH_Temp",TH_Temp);	//��Ӹ���������
	// =========================================================================


	Make_Time(time_buff);
	cJSON_AddStringToObject(send_cjson,"time_flag",time_buff);
	
	str = cJSON_PrintUnformatted(send_cjson);
	strcpy(buf,str);
	cJSON_Delete(send_cjson);
	my_free(str);
	return strlen(buf);
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
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	char buf[256];
	uint8_t body_len = 0;
	printf( "Tips:	OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = Full_Json(buf,mode);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	if(mode!=1)
		Send_queue(buf);
	else
	{
		if(body_len)
		{
			if(MQTT_PacketSaveData(DEVID, body_len, NULL, 3, &mqttPacket) == 0)							//�����$dp
			{
				for(i=0; i < body_len; i++){
					mqttPacket._data[mqttPacket._len++] = buf[i];
				}
				ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
				printf( "Send %d Bytes\r\n", mqttPacket._len);
				
				MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
			}
			else
				printf( "WARN:	EDP_NewBuffer Failed\r\n");
	       
		}
	
	}
/*	
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
*/
}


void OneNet_Send_Device_ID_Data(void)
{
	cJSON *send_cjson = NULL;
    char *str = (char *)my_malloc(1024);
    send_cjson = cJSON_CreateObject();   // ����cjson
    if(send_cjson != NULL)
    {
        cJSON_AddNumberToObject(send_cjson, "state", 0);			//���״ֵ̬
		cJSON_AddStringToObject(send_cjson,"Device_ID",(char *)PROID_Device);
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
		cJSON_AddStringToObject(send_cjson,"Device_ID",(char *)PROID_Device);
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
		cJSON_AddStringToObject(send_cjson,"Device_ID",(char *)PROID_Device);
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
uint8_t a = 0;
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
				
				
				//JSON�ַ�����cJSON��ʽ
				cjson = cJSON_Parse(req_payload); 
				//�ж�cJSON_Parse��������ֵȷ���Ƿ����ɹ�
				if(cjson == NULL){
					  printf( "json pack into cjson error...\r\n");
				}
                // ��������
				else{
                    
						// ===========================================�������յ�������=====================================================
			// =================================== ������־λ =====================================
/*			
//					if((cJSON_GetObjectItem(cjson,"Device_ID"))&&((cJSON_GetObjectItem(cjson,"fire_flag"))||(cJSON_GetObjectItem(cjson,"tem_flag"))||(cJSON_GetObjectItem(cjson,"Hum_flag"))||(cJSON_GetObjectItem(cjson,"light_flag"))||(cJSON_GetObjectItem(cjson,"Gas_flag"))))
//					{
//						
//						if(cJSON_GetObjectItem(cjson,"Device_ID")!=NULL)
//						{
//							DID = cJSON_GetObjectItem(cjson,"Device_ID")->valuestring;
//							printf("csjon�����ɹ� xun_stru.Device_ID = %s\r\n",DID);
//						}
//						
//						
//						if(cJSON_GetObjectItem(cjson,"fire_flag")!=NULL)
//						{
//							fire_flag = cJSON_GetObjectItem(cjson,"fire_flag")->valueint;
//							printf("csjon�����ɹ� fire_flag = %d\r\n",fire_flag);
//							if(fire_flag)
//							{
//								memset(show_buff, 0, sizeof(show_buff));
//								sprintf(show_buff, "%s  ���汨�� ",DID);
//								Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//								if(40+(pot*40)>=740){
//									pot = 1;
//									 LCD_Clear(WHITE);
//								}
//								if(cJSON_GetObjectItem(cjson,"time_flag")!=NULL)
//								{
//									memset(show_buff, 0, sizeof(show_buff));
//									sprintf(show_buff, "��ʼʱ��:%s",cJSON_GetObjectItem(cjson,"time_flag")->valuestring);
//									Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//									if(40+(pot*40)>=740){
//										pot = 1;
//										 LCD_Clear(WHITE);
//									}
//								}
//							}
//							
//						}
//											
//						if(cJSON_GetObjectItem(cjson,"tem_flag")!=NULL)
//						{
//							tem_flag = cJSON_GetObjectItem(cjson,"tem_flag")->valueint;
//							printf("csjon�����ɹ� tem_flag = %d\r\n",tem_flag);
//							if(tem_flag)
//							{
//								memset(show_buff, 0, sizeof(show_buff));
//								sprintf(show_buff, "%s  �¶ȱ������� ",DID);
//								Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//								if(40+(pot*40)>=740){
//									pot = 1;
//									 LCD_Clear(WHITE);
//								}
//								if(cJSON_GetObjectItem(cjson,"time_flag")!=NULL)
//								{
//									memset(show_buff, 0, sizeof(show_buff));
//									sprintf(show_buff, "��ʼʱ��:%s",cJSON_GetObjectItem(cjson,"time_flag")->valuestring);
//									Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//									if(40+(pot*40)>=740){
//										pot = 1;
//										 LCD_Clear(WHITE);
//									}
//								}
//							}
//						}
//						
//						if(cJSON_GetObjectItem(cjson,"Hum_flag")!=NULL)
//						{
//							Hum_flag = cJSON_GetObjectItem(cjson,"Hum_flag")->valueint;
//							printf("csjon�����ɹ� Hum_flag = %d\r\n",Hum_flag);
//							if(Hum_flag)
//							{
//								memset(show_buff, 0, sizeof(show_buff));
//								sprintf(show_buff, "%s  ʪ�ȱ��� ",DID);
//								Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//								if(40+(pot*40)>=740){
//									pot = 1;
//									 LCD_Clear(WHITE);
//								}
//								if(cJSON_GetObjectItem(cjson,"time_flag")!=NULL)
//								{
//									memset(show_buff, 0, sizeof(show_buff));
//									sprintf(show_buff, "��ʼʱ��:%s",cJSON_GetObjectItem(cjson,"time_flag")->valuestring);
//									Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//									if(40+(pot*40)>=740){
//										pot = 1;
//										 LCD_Clear(WHITE);
//									}
//								}
//							}
//						}
//						
//						if(cJSON_GetObjectItem(cjson,"light_flag")!=NULL)
//						{
//							light_flag = cJSON_GetObjectItem(cjson,"light_flag")->valueint;
//							printf("csjon�����ɹ� light_flag = %d\r\n",light_flag);
//							if(light_flag)
//							{
//								memset(show_buff, 0, sizeof(show_buff));
//								sprintf(show_buff, "%s  ���ձ��� ",DID);
//								Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//								if(40+(pot*40)>=740){
//									pot = 1;
//									 LCD_Clear(WHITE);
//								}
//								if(cJSON_GetObjectItem(cjson,"time_flag")!=NULL)
//								{
//									memset(show_buff, 0, sizeof(show_buff));
//									sprintf(show_buff, "��ʼʱ��:%s",cJSON_GetObjectItem(cjson,"time_flag")->valuestring);
//									Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//									if(40+(pot*40)>=740){
//										pot = 1;
//										 LCD_Clear(WHITE);
//									}
//								}
//							}
//						}
//						
//						if(cJSON_GetObjectItem(cjson,"Gas_flag")!=NULL)
//						{
//							Gas_flag = cJSON_GetObjectItem(cjson,"Gas_flag")->valueint;
//							xun_stru.flag = "��������";
//							printf("csjon�����ɹ� Gas_flag = %d\r\n",Gas_flag);
//							if(Gas_flag)
//							{
//								memset(show_buff, 0, sizeof(show_buff));
//								sprintf(show_buff, "%s  �������� ",DID);
//								Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//								if(40+(pot*40)>=740){
//									pot = 1;
//									 LCD_Clear(WHITE);
//								}
//								if(cJSON_GetObjectItem(cjson,"time_flag")!=NULL)
//								{
//									memset(show_buff, 0, sizeof(show_buff));
//									sprintf(show_buff, "��ʼʱ��:%s",cJSON_GetObjectItem(cjson,"time_flag")->valuestring);
//									Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//									if(40+(pot*40)>=740){
//										pot = 1;
//										 LCD_Clear(WHITE);
//									}
//								}
//							}
//						}
//						
//					
//					}
					
*/	
		 // =================================== ============================ =====================================
					
		// =================================== ���������� =====================================
					if(cJSON_GetObjectItem(cjson,"temperature")!=NULL)
					{
						temperature = cJSON_GetObjectItem(cjson,"temperature")->valuedouble;
						printf("csjon�����ɹ� temp_value = %f\r\n",temperature);
					}
					
					if(cJSON_GetObjectItem(cjson,"humidity")!=NULL)
					{
						humidity = cJSON_GetObjectItem(cjson,"humidity")->valuedouble;
						printf("csjon�����ɹ� humidity = %f\r\n",humidity);
					}

					if(cJSON_GetObjectItem(cjson,"Gas")!=NULL)
					{
						Gas = cJSON_GetObjectItem(cjson,"Gas")->valueint;
						printf("csjon�����ɹ� Gas = %d\r\n",Gas);
					}
					
					if(cJSON_GetObjectItem(cjson,"Voltage")!=NULL)
					{
						voltage = cJSON_GetObjectItem(cjson,"Voltage")->valuedouble;
						printf("csjon�����ɹ� Voltage = %.2f\r\n",voltage);
					}
					
					if(cJSON_GetObjectItem(cjson,"Turbidity")!=NULL)
					{
						Turbidity = cJSON_GetObjectItem(cjson,"Turbidity")->valueint;
						printf("csjon�����ɹ� Turbidity = %d\r\n",Turbidity);
					}
					
					if(cJSON_GetObjectItem(cjson,"temperature_ds18")!=NULL)
					{
						temperature_ds18 = cJSON_GetObjectItem(cjson,"temperature_ds18")->valuedouble;
						printf("csjon�����ɹ� temperature_ds18 = %f\r\n",temperature_ds18);
					}
					
					if(cJSON_GetObjectItem(cjson,"state")!=NULL)
					{
						state = cJSON_GetObjectItem(cjson,"state")->valueint;
						printf("csjon�����ɹ� state = %d\r\n",state);
					}
					
					if(cJSON_GetObjectItem(cjson,"user_uing")!=NULL)
					{
						LCD_Clear(WHITE);
						printf("csjon�����ɹ� user_uing = %d\r\n",cJSON_GetObjectItem(cjson,"user_uing")->valueint);
						if(cJSON_GetObjectItem(cjson,"user_uing")->valueint == 4){
							user_uing = 1;
							Make_Time(start_time_buff);
							beep_one();
						}
						
						printf("csjon�����ɹ� user_uing = %d\r\n",user_uing);
					}
					
					if(cJSON_GetObjectItem(cjson,"settlement")!=NULL)
					{
						user_uing = 0;
						OneNet_Send_one_Data(1);   // ����
						beep_one();
					}
					
					
			// ===================================================================================
			
//			if(cJSON_GetObjectItem(cjson,"Device_ID")->valuestring == PROID) // �Զ�ģʽ
//			{				
//					
//			// ============================== ��ֵ���� ==========================================
				if((user_mode == 0)&&(state==2))
				{
					if(cJSON_GetObjectItem(cjson,"TH_Temp")!=NULL)
					{
						TH_Temp = cJSON_GetObjectItem(cjson,"TH_Temp")->valueint;
						printf("csjon�����ɹ� TH_Temp = %d\r\n",TH_Temp);
						at_sava_th();
					}
					
					if(cJSON_GetObjectItem(cjson,"TH_Hum")!=NULL)
					{
						TH_Hum = cJSON_GetObjectItem(cjson,"TH_Hum")->valueint;
						printf("csjon�����ɹ� TH_Temp = %d\r\n",TH_Hum);
						at_sava_th();
					}
					
					if(cJSON_GetObjectItem(cjson,"TH_light")!=NULL)
					{
						TH_light = cJSON_GetObjectItem(cjson,"TH_light")->valueint;
						printf("csjon�����ɹ� TH_light = %d\r\n",TH_light);
						at_sava_th();
					}
					
					
					if(cJSON_GetObjectItem(cjson,"TH_Gas")!=NULL)
					{
						TH_Gas = cJSON_GetObjectItem(cjson,"TH_Gas")->valueint;
						printf("csjon�����ɹ� TH_Gas = %d\r\n",TH_Gas);
						at_sava_th();
					}
				
					
				// ========================== �û����ݴ��� =========================================
					
						// �û�ģʽ
					if(cJSON_GetObjectItem(cjson,"user_mode")!=NULL)
					{
						user_mode = cJSON_GetObjectItem(cjson,"user_mode")->valuedouble;
						printf("csjon�����ɹ� user_mode = %d\r\n",user_mode);
					}
					
					
					if(cJSON_GetObjectItem(cjson,"Led_position")!=NULL)
					{
						Led_position = cJSON_GetObjectItem(cjson,"Led_position")->valueint;
						Led_pwm_mode(Led_position);
						printf("csjon�����ɹ� Led_position = %d\r\n",Led_position);
					}
					
					if(cJSON_GetObjectItem(cjson,"machinery_position")!=NULL)
					{
						machinery_position = cJSON_GetObjectItem(cjson,"machinery_position")->valueint;
						machinery_pwm_mode(machinery_position);
						printf("csjon�����ɹ� machinery_position = %d\r\n",machinery_position);
					}
					
				}
//			}
//				if(a==0)
//				{
//					if(cJSON_GetObjectItem(cjson,"Device_ID")!=NULL)
//					{
//						PROID_Device = cJSON_GetObjectItem(cjson,"Device_ID")->valuestring;
//						printf("csjon�����ɹ� Device_ID = %s\r\n",PROID);
//					}
//					
//				}
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
						state = 2;  
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

