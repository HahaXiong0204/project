/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
#include "dht11.h"
#include "led.h"


//C库
#include <string.h>
#include <stdio.h>

//CJSON库
#include "cJSON.h"



#define PROID		"588384"   //产品ID

#define AUTH_INFO	"123456"		//鉴权信息

#define DEVID		"1064133490"	//设备ID

extern float temperature;
extern unsigned char esp8266_buf[128];
extern u16 water_threshold,tem_threshold;    // 阈值
extern u8 water_flag,fan_flag,heat_flag,xia_water_flag;     // 水泵、风扇、加热器的标志位
extern u16 water_level;
extern s32 Weight_Shiwu;
extern void relay_fan_off(void);
extern void relay_fan_on(void);
extern void relay_shui_off(void);
extern void relay_shui_on(void);
extern void relay_heat_on(void);
extern void relay_heat_off(void);
extern void relay_xia_shui_off(void);
extern void relay_xia_shui_on(void);

extern u8 tea;

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
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
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
					case 0:UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法的clientid\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法链接(比如token非法)\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
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
		UsartPrintf(USART_DEBUG, "Subscribe Topic: %s\r\n", topics[i]);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL2, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求
		
		MQTT_DeleteBuffer(&mqttPacket);											//删包
	}

}



unsigned char OneNet_FillBuf(char *buf)
{   
	char text[64];
    printf("发送\r\n");
    


	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{");
	memset(text, 0, sizeof(text));
	sprintf(text, "\"temperature\":%.3f,", (float)temperature);   // 温度
	strcat(buf, text);
    
    memset(text, 0, sizeof(text));
	sprintf(text, "\"tea\":%d,", tea);   // 温度
	strcat(buf, text);

    memset(text, 0, sizeof(text));
	sprintf(text, "\"water_level\":%d,", water_level);   // 水位
	strcat(buf, text);
    
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Weight_Shiwu\":%d,", Weight_Shiwu);   // 重量
	strcat(buf, text);
    
    
    memset(text, 0, sizeof(text));   // 发送水泵
    if(water_flag==0){
    	sprintf(text, "\"water_pump\":OFF,");
        
    }else{
        sprintf(text, "\"water_pump\":ON,");
    }
	strcat(buf, text);
    
    
    memset(text, 0, sizeof(text));  //发送风扇
    if(fan_flag==0){
    	sprintf(text, "\"fan\":OFF,");
        
    }else{
        sprintf(text, "\"fan\":ON,");
    }
	strcat(buf, text);
    
    
    memset(text, 0, sizeof(text));  //发送加热
    if(heat_flag==0)
    {
    	sprintf(text, "\"heater\":OFF,");
        
    }else
    {
        sprintf(text, "\"heater\":ON,");
    }
	strcat(buf, text);
    
    
    memset(text, 0, sizeof(text));  //发送上水标志位
    if(water_flag==0)
    {
    	sprintf(text, "\"shang_water\":OFF,");
        
    }else
    {
        sprintf(text, "\"shang_water\":ON,");
    }
	strcat(buf, text);

    memset(text, 0, sizeof(text));  //发送下水标志位
    if(xia_water_flag==0)
    {
    	sprintf(text, "\"xia_water\":OFF,");
        
    }else
    {
        sprintf(text, "\"xia_water\":ON,");
    }
	strcat(buf, text);

    
    memset(text, 0, sizeof(text));     //水位阈值
	sprintf(text, "\"water_threshold\":%d,", water_threshold);
	strcat(buf, text);
    
    memset(text, 0, sizeof(text));   // 温度阈值
	sprintf(text, "\"tem_threshold\":%d", tem_threshold);
	strcat(buf, text);
	
	strcat(buf, "}");
	
	return strlen(buf);
	

}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[256];
	
	short body_len = 0, i = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);																	//获取当前需要发送的数据流的总长度
	
	if(body_len)
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 3, &mqttPacket) == 0)							//封包
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
			UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
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
	char numBuf[10];
	int num = 0;
	
	cJSON* cjson;
	int value;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
				
				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
				{
					UsartPrintf(USART_DEBUG, "Tips:	Send CmdResp\r\n");
					
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//回复命令
					MQTT_DeleteBuffer(&mqttPacket);									//删包
				}
			}
		
		break;
			
		case MQTT_PKT_PUBLISH:														//接收的Publish消息
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				UsartPrintf(USART_DEBUG, "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
				
				
				
				//JSON字符串到cJSON格式
				cjson = cJSON_Parse(req_payload); 
				//判断cJSON_Parse函数返回值确定是否打包成功
				if(cjson == NULL){
//						printf("json pack into cjson error...");
					  UsartPrintf(USART_DEBUG, "json pack into cjson error...\r\n");
				}
				else{
					//获取字段值
				//cJSON_GetObjectltem返回的是一个cJSON结构体所以我们可以通过函数返回结构体的方式选择返回类型！
				value = cJSON_GetObjectItem(cjson,"fan")->valueint;
				
				if(value){
                    relay_fan_on();
                    printf("风扇开启\r\n");
                }
				else 
                {
                    relay_fan_off();
                    printf("风扇关闭\r\n");
                }
//
				value = cJSON_GetObjectItem(cjson,"xia_water")->valueint;
				
				if(value){
                    relay_xia_shui_on();
                    printf("下水泵开启\r\n");
                }
				else 
                {
                    relay_xia_shui_off();
                    printf("下水泵关闭\r\n");
                }

                
                value = cJSON_GetObjectItem(cjson,"water_pump")->valueint;
				
				if(value==1){
                    relay_shui_on();
                    printf("水泵开启\r\n");
                }
				else 
                {
                    relay_shui_off();
                    printf("水泵关闭\r\n");
                }
                
               value = cJSON_GetObjectItem(cjson,"heater")->valueint;
				
				if(value){
                    relay_heat_on();
                    printf("加热器开启\r\n");
                }
				else 
                {
                    relay_heat_off();
                    printf("加热器关闭\r\n");
                }
                
                value = atoi(cJSON_GetObjectItem(cjson,"tem_threshold")->valuestring);
                if(value!=0)
                {
                    tem_threshold = value;
                }
                value = atoi(cJSON_GetObjectItem(cjson,"water_threshold")->valuestring);
                if(value!=0)
                {
                    water_threshold = value;
                }
//				
					
				}
				 
				//delete cjson
				cJSON_Delete(cjson);

				
				
				switch(qos)
				{
					case 1:															//收到publish的qos为1，设备需要回复Ack
					
						if(MQTT_PacketPublishAck(pkt_id, &mqttPacket) == 0)
						{
							UsartPrintf(USART_DEBUG, "Tips:	Send PublishAck\r\n");
							ESP8266_SendData(mqttPacket._data, mqttPacket._len);
							MQTT_DeleteBuffer(&mqttPacket);
						}
					
					break;
					
					case 2:															//收到publish的qos为2，设备先回复Rec
																					//平台回复Rel，设备再回复Comp
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
			}
		
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");
			
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
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;
		
		num = atoi((const char *)numBuf);				//转为数值形式
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
