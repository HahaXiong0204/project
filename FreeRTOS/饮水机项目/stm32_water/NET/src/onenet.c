//单片机头文件
#include "main.h"

//网络设备
#include "wifi.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"



//C库
#include <string.h>
#include <stdio.h>

//CJSON库
#include "cJSON.h"

#include "delay.h"
//#include "24cxx.h"


uint8_t *PROID_Device = PROID; 


extern const char *topics[];      // 订阅的主题





//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	printf( "OneNet_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
                        , PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:printf( "Tips:	连接成功\r\n");status = 0;break;
					
					case 1:printf( "WARN:	连接失败：协议错误\r\n");break;
					case 2:printf( "WARN:	连接失败：非法的clientid\r\n");break;
					case 3:printf( "WARN:	连接失败：服务器失败\r\n");break;
					case 4:printf( "WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:printf( "WARN:	连接失败：非法链接(比如token非法)\r\n");break;
					
					default:printf( "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
		HAL_Delay(500);
		OneNet_Subscribe(topics, 1);   // 订阅主题
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
	发送数据
mode:
	1 
*/
static char one_buff[256];
void OneNet_Send_one_Data(uint8_t mode)
{
	cJSON *send_cjson = NULL;
	char *str = (char*)my_malloc(512);
	send_cjson = cJSON_CreateObject();   // 创建cjson
	memset(one_buff,0,sizeof(one_buff));
	/*
			cJSON_AddStringToObject(send_cjson,"name","MQ");		//添加字符串 
			cJSON_AddNumberToObject(send_cjson,"age",25);	    	//添加整型数字 
			cJSON_AddNumberToObject(send_cjson,"height",183.52);	//添加浮点型数字
			cJSON_AddFalseToObject(send_cjson,"gender");			//添加逻辑值false
	*/
	cJSON_AddStringToObject(send_cjson,"Device_ID",PROID);
	if(mode == 1)
	{
		cJSON_AddNumberToObject(send_cjson,"settlement",1);	//添加浮点型数字
		cJSON_AddNumberToObject(send_cjson,"Water_all",Water_all);	//添加浮点型数字
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



// 数据上传封装函数  mode:0 正常   mode:1 重发
int id =0;
uint8_t Full_Json(char *buf,uint8_t mode)
{
	cJSON *send_cjson = NULL;
	char *str = (char*)my_malloc(512);
	send_cjson = cJSON_CreateObject();   // 创建cjson
	//	cJSON_AddStringToObject(send_cjson,"name","MQ");		//添加字符串 
//	cJSON_AddNumberToObject(send_cjson,"age",25);	    	//添加整型数字 
//	cJSON_AddNumberToObject(send_cjson,"height",183.52);	//添加浮点型数字
//	cJSON_AddFalseToObject(send_cjson,"gender");			//添加逻辑值false

	if(mode == 1)
	{
		cJSON_AddNumberToObject(send_cjson,"Again_flag",1);		//添加字符串 
		
	}
	cJSON_AddStringToObject(send_cjson,"Device_ID",PROID);
	
	
		// =============================  传感器数据 ==============================
		cJSON_AddNumberToObject(send_cjson,"temperature",temperature);	//添加浮点型数字
		cJSON_AddNumberToObject(send_cjson,"hot_temperature",Water_Hot);	//添加浮点型数字
		cJSON_AddNumberToObject(send_cjson,"Turbidity1",Water_TDS1);	//添加浮点型数字
		cJSON_AddNumberToObject(send_cjson,"Turbidity2",Water_TDS2);	//添加浮点型数字
		cJSON_AddNumberToObject(send_cjson,"TH_Temp",TH_Temp);	//添加浮点型数字
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
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//			mode:0 正常   mode:1 重发
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(uint8_t mode)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	char buf[256];
	uint8_t body_len = 0;
	printf( "Tips:	OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = Full_Json(buf,mode);																	//获取当前需要发送的数据流的总长度
	
	if(mode!=1)
		Send_queue(buf);
	else
	{
		if(body_len)
		{
			if(MQTT_PacketSaveData(DEVID, body_len, NULL, 3, &mqttPacket) == 0)							//封包给$dp
			{
				for(i=0; i < body_len; i++){
					mqttPacket._data[mqttPacket._len++] = buf[i];
				}
				ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
				printf( "Send %d Bytes\r\n", mqttPacket._len);
				
				MQTT_DeleteBuffer(&mqttPacket);															//删包
			}
			else
				printf( "WARN:	EDP_NewBuffer Failed\r\n");
	       
		}
	
	}
/*	
//	Wait_send_mutex();
//	if(body_len)
//	{
//		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 3, &mqttPacket) == 0)							//封包给$dp
//		{
//			for(i=0; i < body_len; i++){
//				mqttPacket._data[mqttPacket._len++] = buf[i];
//			}
//			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
//			printf( "Send %d Bytes\r\n", mqttPacket._len);
//			
//			MQTT_DeleteBuffer(&mqttPacket);															//删包
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
    send_cjson = cJSON_CreateObject();   // 创建cjson
    if(send_cjson != NULL)
    {
        cJSON_AddNumberToObject(send_cjson, "state", 0);			//添加状态值
		cJSON_AddStringToObject(send_cjson,"Device_ID",(char *)PROID_Device);
        str = cJSON_PrintUnformatted(send_cjson);
        cJSON_Delete(send_cjson);		// 释放内存
		Send_queue(str);
        my_free(str);

    }

}

void OneNet_Send_Int(uint8_t *key ,uint16_t value)
{
	cJSON *send_cjson = NULL;
    char *str = (char *)my_malloc(1024);
    send_cjson = cJSON_CreateObject();   // 创建cjson
    if(send_cjson != NULL)
    {
		cJSON_AddNumberToObject(send_cjson,(char*)key,value);
		cJSON_AddStringToObject(send_cjson,"Device_ID",(char *)PROID_Device);
		str = cJSON_PrintUnformatted(send_cjson);
		cJSON_Delete(send_cjson);		// 释放内存
		Send_queue(str);
        my_free(str);
	}
}

void OneNet_Send_String(uint8_t *key ,uint8_t *value)
{
	cJSON *send_cjson = NULL;
    char *str = (char *)my_malloc(1024);
    send_cjson = cJSON_CreateObject();   // 创建cjson
    if(send_cjson != NULL)
    {
		cJSON_AddStringToObject(send_cjson,(char*)key,(char*)value);
		cJSON_AddStringToObject(send_cjson,"Device_ID",(char *)PROID_Device);
		str = cJSON_PrintUnformatted(send_cjson);
		cJSON_Delete(send_cjson);		// 释放内存
		Send_queue(str);
        my_free(str);
	}
}



//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
uint8_t a = 0;
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	uint8_t pianyi = 0;   // dataPtr的偏移量
	char numBuf[10];
	int num = 0;
	
	char *buff_time;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
		{
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				printf( "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
				{
					printf( "Tips:	Send CmdResp\r\n");
					
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//回复命令
					MQTT_DeleteBuffer(&mqttPacket);									//删包
				}
			}
		
		break;
		}
		case MQTT_PKT_PUBLISH:														//接收的Publish消息
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				printf( "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
				
				cJSON *cjson = NULL;
				
				
				//JSON字符串到cJSON格式
				cjson = cJSON_Parse(req_payload); 
				//判断cJSON_Parse函数返回值确定是否打包成功
				if(cjson == NULL){
					  printf( "json pack into cjson error...\r\n");
				}
                // 处理数据
				else{
                    
						// ===========================================解析接收到的数据=====================================================
			// =================================== 报警标志位 =====================================
/*			
//					if((cJSON_GetObjectItem(cjson,"Device_ID"))&&((cJSON_GetObjectItem(cjson,"fire_flag"))||(cJSON_GetObjectItem(cjson,"tem_flag"))||(cJSON_GetObjectItem(cjson,"Hum_flag"))||(cJSON_GetObjectItem(cjson,"light_flag"))||(cJSON_GetObjectItem(cjson,"Gas_flag"))))
//					{
//						
//						if(cJSON_GetObjectItem(cjson,"Device_ID")!=NULL)
//						{
//							DID = cJSON_GetObjectItem(cjson,"Device_ID")->valuestring;
//							printf("csjon解析成功 xun_stru.Device_ID = %s\r\n",DID);
//						}
//						
//						
//						if(cJSON_GetObjectItem(cjson,"fire_flag")!=NULL)
//						{
//							fire_flag = cJSON_GetObjectItem(cjson,"fire_flag")->valueint;
//							printf("csjon解析成功 fire_flag = %d\r\n",fire_flag);
//							if(fire_flag)
//							{
//								memset(show_buff, 0, sizeof(show_buff));
//								sprintf(show_buff, "%s  火焰报警 ",DID);
//								Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//								if(40+(pot*40)>=740){
//									pot = 1;
//									 LCD_Clear(WHITE);
//								}
//								if(cJSON_GetObjectItem(cjson,"time_flag")!=NULL)
//								{
//									memset(show_buff, 0, sizeof(show_buff));
//									sprintf(show_buff, "开始时间:%s",cJSON_GetObjectItem(cjson,"time_flag")->valuestring);
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
//							printf("csjon解析成功 tem_flag = %d\r\n",tem_flag);
//							if(tem_flag)
//							{
//								memset(show_buff, 0, sizeof(show_buff));
//								sprintf(show_buff, "%s  温度报警报警 ",DID);
//								Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//								if(40+(pot*40)>=740){
//									pot = 1;
//									 LCD_Clear(WHITE);
//								}
//								if(cJSON_GetObjectItem(cjson,"time_flag")!=NULL)
//								{
//									memset(show_buff, 0, sizeof(show_buff));
//									sprintf(show_buff, "开始时间:%s",cJSON_GetObjectItem(cjson,"time_flag")->valuestring);
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
//							printf("csjon解析成功 Hum_flag = %d\r\n",Hum_flag);
//							if(Hum_flag)
//							{
//								memset(show_buff, 0, sizeof(show_buff));
//								sprintf(show_buff, "%s  湿度报警 ",DID);
//								Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//								if(40+(pot*40)>=740){
//									pot = 1;
//									 LCD_Clear(WHITE);
//								}
//								if(cJSON_GetObjectItem(cjson,"time_flag")!=NULL)
//								{
//									memset(show_buff, 0, sizeof(show_buff));
//									sprintf(show_buff, "开始时间:%s",cJSON_GetObjectItem(cjson,"time_flag")->valuestring);
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
//							printf("csjon解析成功 light_flag = %d\r\n",light_flag);
//							if(light_flag)
//							{
//								memset(show_buff, 0, sizeof(show_buff));
//								sprintf(show_buff, "%s  光照报警 ",DID);
//								Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//								if(40+(pot*40)>=740){
//									pot = 1;
//									 LCD_Clear(WHITE);
//								}
//								if(cJSON_GetObjectItem(cjson,"time_flag")!=NULL)
//								{
//									memset(show_buff, 0, sizeof(show_buff));
//									sprintf(show_buff, "开始时间:%s",cJSON_GetObjectItem(cjson,"time_flag")->valuestring);
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
//							xun_stru.flag = "空气报警";
//							printf("csjon解析成功 Gas_flag = %d\r\n",Gas_flag);
//							if(Gas_flag)
//							{
//								memset(show_buff, 0, sizeof(show_buff));
//								sprintf(show_buff, "%s  空气报警 ",DID);
//								Show_Str(10,40+(pot++*40),560,32,(uint8_t *)show_buff,32,0);
//								if(40+(pot*40)>=740){
//									pot = 1;
//									 LCD_Clear(WHITE);
//								}
//								if(cJSON_GetObjectItem(cjson,"time_flag")!=NULL)
//								{
//									memset(show_buff, 0, sizeof(show_buff));
//									sprintf(show_buff, "开始时间:%s",cJSON_GetObjectItem(cjson,"time_flag")->valuestring);
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
					
		// =================================== 传感器数据 =====================================
					if(cJSON_GetObjectItem(cjson,"temperature")!=NULL)
					{
						temperature = cJSON_GetObjectItem(cjson,"temperature")->valuedouble;
						printf("csjon解析成功 temp_value = %f\r\n",temperature);
					}
					
					if(cJSON_GetObjectItem(cjson,"humidity")!=NULL)
					{
						humidity = cJSON_GetObjectItem(cjson,"humidity")->valuedouble;
						printf("csjon解析成功 humidity = %f\r\n",humidity);
					}

					if(cJSON_GetObjectItem(cjson,"Gas")!=NULL)
					{
						Gas = cJSON_GetObjectItem(cjson,"Gas")->valueint;
						printf("csjon解析成功 Gas = %d\r\n",Gas);
					}
					
					if(cJSON_GetObjectItem(cjson,"Voltage")!=NULL)
					{
						voltage = cJSON_GetObjectItem(cjson,"Voltage")->valuedouble;
						printf("csjon解析成功 Voltage = %.2f\r\n",voltage);
					}
					
					if(cJSON_GetObjectItem(cjson,"Turbidity")!=NULL)
					{
						Turbidity = cJSON_GetObjectItem(cjson,"Turbidity")->valueint;
						printf("csjon解析成功 Turbidity = %d\r\n",Turbidity);
					}
					
					if(cJSON_GetObjectItem(cjson,"temperature_ds18")!=NULL)
					{
						temperature_ds18 = cJSON_GetObjectItem(cjson,"temperature_ds18")->valuedouble;
						printf("csjon解析成功 temperature_ds18 = %f\r\n",temperature_ds18);
					}
					
					if(cJSON_GetObjectItem(cjson,"state")!=NULL)
					{
						state = cJSON_GetObjectItem(cjson,"state")->valueint;
						printf("csjon解析成功 state = %d\r\n",state);
					}
					
					if(cJSON_GetObjectItem(cjson,"user_uing")!=NULL)
					{
						LCD_Clear(WHITE);
						printf("csjon解析成功 user_uing = %d\r\n",cJSON_GetObjectItem(cjson,"user_uing")->valueint);
						if(cJSON_GetObjectItem(cjson,"user_uing")->valueint == 4){
							user_uing = 1;
							Make_Time(start_time_buff);
							beep_one();
						}
						
						printf("csjon解析成功 user_uing = %d\r\n",user_uing);
					}
					
					if(cJSON_GetObjectItem(cjson,"settlement")!=NULL)
					{
						user_uing = 0;
						OneNet_Send_one_Data(1);   // 结算
						beep_one();
					}
					
					
			// ===================================================================================
			
//			if(cJSON_GetObjectItem(cjson,"Device_ID")->valuestring == PROID) // 自动模式
//			{				
//					
//			// ============================== 阈值处理 ==========================================
				if((user_mode == 0)&&(state==2))
				{
					if(cJSON_GetObjectItem(cjson,"TH_Temp")!=NULL)
					{
						TH_Temp = cJSON_GetObjectItem(cjson,"TH_Temp")->valueint;
						printf("csjon解析成功 TH_Temp = %d\r\n",TH_Temp);
						at_sava_th();
					}
					
					if(cJSON_GetObjectItem(cjson,"TH_Hum")!=NULL)
					{
						TH_Hum = cJSON_GetObjectItem(cjson,"TH_Hum")->valueint;
						printf("csjon解析成功 TH_Temp = %d\r\n",TH_Hum);
						at_sava_th();
					}
					
					if(cJSON_GetObjectItem(cjson,"TH_light")!=NULL)
					{
						TH_light = cJSON_GetObjectItem(cjson,"TH_light")->valueint;
						printf("csjon解析成功 TH_light = %d\r\n",TH_light);
						at_sava_th();
					}
					
					
					if(cJSON_GetObjectItem(cjson,"TH_Gas")!=NULL)
					{
						TH_Gas = cJSON_GetObjectItem(cjson,"TH_Gas")->valueint;
						printf("csjon解析成功 TH_Gas = %d\r\n",TH_Gas);
						at_sava_th();
					}
				
					
				// ========================== 用户数据处理 =========================================
					
						// 用户模式
					if(cJSON_GetObjectItem(cjson,"user_mode")!=NULL)
					{
						user_mode = cJSON_GetObjectItem(cjson,"user_mode")->valuedouble;
						printf("csjon解析成功 user_mode = %d\r\n",user_mode);
					}
					
					
					if(cJSON_GetObjectItem(cjson,"Led_position")!=NULL)
					{
						Led_position = cJSON_GetObjectItem(cjson,"Led_position")->valueint;
						Led_pwm_mode(Led_position);
						printf("csjon解析成功 Led_position = %d\r\n",Led_position);
					}
					
					if(cJSON_GetObjectItem(cjson,"machinery_position")!=NULL)
					{
						machinery_position = cJSON_GetObjectItem(cjson,"machinery_position")->valueint;
						machinery_pwm_mode(machinery_position);
						printf("csjon解析成功 machinery_position = %d\r\n",machinery_position);
					}
					
				}
//			}
//				if(a==0)
//				{
//					if(cJSON_GetObjectItem(cjson,"Device_ID")!=NULL)
//					{
//						PROID_Device = cJSON_GetObjectItem(cjson,"Device_ID")->valuestring;
//						printf("csjon解析成功 Device_ID = %s\r\n",PROID);
//					}
//					
//				}
					/*
						extern uint16_t year_time;
						extern uint8_t moon_time;
						extern uint8_t sun_time;
						extern uint8_t hour_time;   // 小时
						extern uint8_t minute_time;  // 分钟
						extern uint8_t second_time;  // 秒
					*/
					if(cJSON_GetObjectItem(cjson,"time")!=NULL)
					{
						buff_time = cJSON_GetObjectItem(cjson,"time")->valuestring;
						// 23-3-4-4-4-
						sscanf(buff_time,"%d-%d-%d-%d-%d-%d",&year_time,&moon_time,&sun_time,&hour_time,&minute_time,&second_time);
						time_init(year_time,moon_time,sun_time,hour_time,minute_time,second_time);
						state = 2;  
						printf("time csjon解析成功\r\n");
					}
					
					

					//======================================================================================

				}
				 
					//delete cjson

				cJSON_Delete(cjson);
				printf("------------------------------------释放成功-----------------------------------------\r\n");
				
				switch(qos)
				{
					case 1:															//收到publish的qos为1，设备需要回复Ack
					
						if(MQTT_PacketPublishAck(pkt_id, &mqttPacket) == 0)
						{
							printf( "Tips:	Send PublishAck\r\n");
							ESP8266_SendData(mqttPacket._data, mqttPacket._len);
							MQTT_DeleteBuffer(&mqttPacket);
						}
					
					break;
					
					case 2:															//收到publish的qos为2，设备先回复Rec
																					//平台回复Rel，设备再回复Comp
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
                ESP8266_Clear();									//清空缓存
				if(dataPtr!=NULL)
				{
					my_free(dataPtr);
					dataPtr = NULL;
					printf("释放dataptr空间成功\r\n");
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
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				printf( "Tips:	MQTT Publish Send OK\r\n");
			
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//搜索'}'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;
		pianyi++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
			pianyi++;
		}
		numBuf[num] = 0;
		printf("09 -============\r\n");
        
		num = atoi((const char *)numBuf);				//转为数值形式

//        //=================$crep接收的处理start================
        result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
        
        printf( "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);


        
        //=================$crep接收处理end=====================
        
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
	dataPtr -= pianyi;
	
		
		
}

//==========================================================
//	函数名称：	OneNet_Publish
//
//	函数功能：	发布消息
//
//	入口参数：	topic：发布的主题
//				msg：消息内容
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_PUBLISH-需要重送
//
//	说明：		
//==========================================================
void OneNet_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	/*返回非0表示未能给mqttPacket协议包完全填充数据*/
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
        printf("Publish Topic: %s, Msg: %s\r\n", topic, msg);

		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}
    else{
    	printf("Publish Error!\r\n");
    }

}



//==========================================================
//	函数名称：	OneNet_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	topics：订阅的topic
//				topic_cnt：topic个数
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_SUBSCRIBE-需要重发
//
//	说明：		
//==========================================================
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//协议包
	
	for(; i < topic_cnt; i++)
		printf( "Subscribe Topic: %s\r\n", topics[i]);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL2, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}

