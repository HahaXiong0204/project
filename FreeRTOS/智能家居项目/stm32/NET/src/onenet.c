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


#define PROID		"588384"   //产品ID

#define AUTH_INFO	"123456"		//鉴权信息

#define DEVID		"1064133490"	//设备ID





extern uint8_t mode;

extern double temperature;               //温度
extern double humidity;                  //湿度
extern uint32_t Lighting, Turbidity, Gas; //光照，浊度，烟雾的数据
extern float voltage;  // 电压


// 数据上传封装函数  buf：数据  flag：1添加重发符号  0:不添加重发符号
uint16_t id = 0 ;
uint16_t OneNet_FillBuf(char *buf, uint8_t again_flag)
{
    cJSON *send_cjson = NULL;
    char *str = (char *)malloc(1024);
    send_cjson = cJSON_CreateObject();   // 创建cjson

    if(send_cjson != NULL)
    {
        printf("准备组装json\r\n");
        //	cJSON_AddStringToObject(send_cjson,"name","MQ");		//添加字符串
        //	cJSON_AddNumberToObject(send_cjson,"age",25);	    	//添加整型数字
        //	cJSON_AddNumberToObject(send_cjson,"height",183.52);	//添加浮点型数字
        //	cJSON_AddFalseToObject(send_cjson,"gender");			//添加逻辑值false

        if(again_flag)
            cJSON_AddNumberToObject(send_cjson, "again_flag", 1); // 重发标志位

        cJSON_AddNumberToObject(send_cjson, "age", id++);	    	//添加整型数字
		cJSON_AddNumberToObject(send_cjson, "Beep", beep_flag);	    	//添加整型数字
		cJSON_AddNumberToObject(send_cjson, "state", mode_flag);	    	//添加整型数字
        cJSON_AddNumberToObject(send_cjson, "tem", temperature);	//添加浮点型数字
        cJSON_AddNumberToObject(send_cjson, "hum", humidity);	//添加浮点型数字
        cJSON_AddNumberToObject(send_cjson, "light", Lighting);	//添加浮点型数字
        cJSON_AddNumberToObject(send_cjson, "TH_tem", TH_Temp);	//添加浮点型数字
        cJSON_AddNumberToObject(send_cjson, "TH_light", TH_light);	//添加浮点型数字
		cJSON_AddNumberToObject(send_cjson, "TH_Gas", TH_Gas);	//添加浮点型数字
		cJSON_AddNumberToObject(send_cjson, "TH_hum", TH_Hum);	//添加浮点型数字
		

        str = cJSON_PrintUnformatted(send_cjson);
        printf("STR = %s\r\n", str);
        strcpy(buf, str);
        free(str);
        cJSON_Delete(send_cjson);
        return strlen(buf);
    }

    return 0;


}

void OneNet_SendData_State(char *buf)
{
    cJSON *send_cjson = NULL;
    char *str = (char *)malloc(1024);
    send_cjson = cJSON_CreateObject();   // 创建cjson
    if(send_cjson != NULL)
    {
        cJSON_AddStringToObject(send_cjson, "STATUE", buf);			//添加状态值
        str = cJSON_PrintUnformatted(send_cjson);
        printf("STR = %s\r\n", str);
        cJSON_Delete(send_cjson);		// 释放内存
        OneNet_SendData_Buff(str);
        free(str);

    }
}


void OneNet_Send_One_String_Data(char *key,char *value)
{
    cJSON *send_cjson = NULL;
    char *str = (char *)malloc(1024);
    send_cjson = cJSON_CreateObject();   // 创建cjson
    if(send_cjson != NULL)
    {
        cJSON_AddStringToObject(send_cjson, key, value);			//添加状态值
        str = cJSON_PrintUnformatted(send_cjson);
        printf("STR = %s\r\n", str);
        cJSON_Delete(send_cjson);		// 释放内存
        OneNet_SendData_Buff(str);
        free(str);

    }
}

void OneNet_Send_One_Int_Data(char *key,uint16_t value)
{
	 cJSON *send_cjson = NULL;
    char *str = (char *)malloc(1024);
    send_cjson = cJSON_CreateObject();   // 创建cjson
    if(send_cjson != NULL)
    {
        cJSON_AddNumberToObject(send_cjson, key, value);			//添加状态值
        str = cJSON_PrintUnformatted(send_cjson);
        printf("STR = %s\r\n", str);
        cJSON_Delete(send_cjson);		// 释放内存
        OneNet_SendData_Buff(str);
        free(str);

    }


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

    //	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包

    char buf[256];

    //	short body_len = 0;

    printf( "Tips:	OneNet_SendData-MQTT\r\n");

    memset(buf, 0, sizeof(buf));

    OneNet_FillBuf(buf, 0);																	//获取当前需要发送的数据流的总长度

    OneNet_SendData_Buff(buf);

}

// return 1 成功
// return 0 失败
uint8_t OneNet_SendData_Buff(char *buf)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
    uint16_t body_len = strlen(buf);
    uint16_t i = 0;
    if(body_len)
    {
        if(MQTT_PacketSaveData(DEVID, body_len, NULL, 3, &mqttPacket) == 0)							//封包给$dp
        {
            for(i = 0; i < body_len; i++)
            {
                mqttPacket._data[mqttPacket._len++] = buf[i];
            }
            ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
            printf( "Send %d Bytes\r\n", mqttPacket._len);

            MQTT_DeleteBuffer(&mqttPacket);															//删包
            return 1;
        }
        else
            printf( "WARN:	EDP_NewBuffer Failed\r\n");

    }
    return 0;
}

void OneNet_Send_Device_ID_Data(void)
{
	cJSON *send_cjson = NULL;
    char *str = (char *)malloc(1024);
    send_cjson = cJSON_CreateObject();   // 创建cjson
    if(send_cjson != NULL)
    {
        cJSON_AddNumberToObject(send_cjson, "state", 0);			//添加状态值
		cJSON_AddNumberToObject(send_cjson,"Device_ID",Device_ID);
        str = cJSON_PrintUnformatted(send_cjson);
        printf("STR = %s\r\n", str);
        cJSON_Delete(send_cjson);		// 释放内存
//        OneNet_SendData_Buff(str);
		OneNet_Publish("one",str);
        free(str);

    }

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
                case 0:
                    printf( "Tips:	连接成功\r\n");
                    status = 0;
                    break;

                case 1:
                    printf( "WARN:	连接失败：协议错误\r\n");
                    break;
                case 2:
                    printf( "WARN:	连接失败：非法的clientid\r\n");
                    break;
                case 3:
                    printf( "WARN:	连接失败：服务器失败\r\n");
                    break;
                case 4:
                    printf( "WARN:	连接失败：用户名或密码错误\r\n");
                    break;
                case 5:
                    printf( "WARN:	连接失败：非法链接(比如token非法)\r\n");
                    break;

                default:
                    printf( "ERR:	连接失败：未知错误\r\n");
                    break;
                }
            }
        }
		sprintf(Publish_Topc,"device/%d/post",Device_ID);      //  拼接设备发布主题
		
		sprintf(Submit_Topc,"device/%d/sub",Device_ID);
		
		printf("sub = %s\r\n",Submit_Topc);
		
		
		
        MQTT_DeleteBuffer(&mqttPacket);								//删包
        OneNet_Subscribe(topics, 1);   // 订阅主题
		HAL_Delay(500);
		OneNet_Send_Device_ID_Data();
		
		
    }
    else
        printf( "WARN:	MQTT_PacketConnect Failed\r\n");

	

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
        printf( "Subscribe Topic: %s\r\n", topics[i]);

    if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL2, topics, topic_cnt, &mqttPacket) == 0)
    {
        ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//向平台发送订阅请求

        MQTT_DeleteBuffer(&mqttPacket);											//删包
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
    uint8_t pianyi = 0;   // dataPtr的偏移量
    char numBuf[10];
    int num = 0;


    //	int value;

    type = MQTT_UnPacketRecv(cmd);
    switch(type)
    {
    case MQTT_PKT_CMD:															//命令下发

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

    case MQTT_PKT_PUBLISH:														//接收的Publish消息
        printf("接收数据MQTT_PKT_PUBLISH\r\n");
        result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
        if(result == 0)
        {
            printf( "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
                    cmdid_topic, topic_len, req_payload, req_len);

            cJSON *cjson = NULL;

            //测试参数
            double temp_value, humi_value;

            //JSON字符串到cJSON格式
            cjson = cJSON_Parse(req_payload);
            //判断cJSON_Parse函数返回值确定是否打包成功
            if(cjson == NULL)
            {
                //						printf("json pack into cjson error...");
                printf( "json pack into cjson error...\r\n");
            }
            // 处理one的数据
            else
            {

                //获取字段值
                //cJSON_GetObjectltem返回的是一个cJSON结构体所以我们可以通过函数返回结构体的方式选择返回类型！

                uint8_t flag = 0;
                printf("cjson 内存大小为 = %d\r\n", sizeof(cjson));
                // ===========================================解析接收到的数据=====================================================
                if(cJSON_GetObjectItem(cjson, "state") != NULL)
				{
					mode_flag = cJSON_GetObjectItem(cjson, "state")->valueint;
					printf("csjon解析成功 temp_value = %d\r\n", mode_flag);
				}
				
				if(mode_flag==1)
				{
					if(cJSON_GetObjectItem(cjson, "TH_tem") != NULL)
					{
						TH_Temp = cJSON_GetObjectItem(cjson, "TH_tem")->valueint;
						printf("csjon解析成功 temp_value = %d\r\n", TH_Temp);
					}
					if(cJSON_GetObjectItem(cjson, "TH_light") != NULL)
					{
						TH_light = cJSON_GetObjectItem(cjson, "TH_light")->valueint;
						printf("csjon解析成功 temp_value = %d\r\n", TH_light);
					}
					if(cJSON_GetObjectItem(cjson, "TH_hum") != NULL)
					{
						TH_Hum = cJSON_GetObjectItem(cjson, "TH_hum")->valueint;
						printf("csjon解析成功 temp_value = %d\r\n", TH_Hum);
					}
					if(cJSON_GetObjectItem(cjson, "TH_Gas") != NULL)
					{
						TH_Gas = cJSON_GetObjectItem(cjson, "TH_Gas")->valueint;
						printf("csjon解析成功 temp_value = %d\r\n", TH_Gas);
					}
					
				}
				
				
				

                if(cJSON_GetObjectItem(cjson, "flag") != NULL)
                {
                    flag = cJSON_GetObjectItem(cjson, "flag")->valueint;
                    printf("csjon解析成功 flag = %d\r\n", flag);
                }

                if(cJSON_GetObjectItem(cjson, "Humidity") != NULL)
                {
                    humi_value = cJSON_GetObjectItem(cjson, "Humidity")->valuedouble;
                    printf("csjon解析成功 Humidity = %f\r\n", humi_value);
                }
				
                if(cJSON_GetObjectItem(cjson, "state") != NULL)
                {
                    mode_flag = cJSON_GetObjectItem(cjson, "state")->valueint;
                    printf("csjon解析成功 state = %d\r\n", mode_flag);
                }
				

                /*
                	extern uint16_t year_time;
                	extern uint8_t moon_time;
                	extern uint8_t sun_time;
                	extern uint8_t hour_time;   // 小时
                	extern uint8_t minute_time;  // 分钟
                	extern uint8_t second_time;  // 秒
                */
                if(cJSON_GetObjectItem(cjson, "time") != NULL)
                {
                    if(cJSON_GetObjectItem(cjson, "time")->valueint)
                    {
                        year_time = cJSON_GetObjectItem(cjson, "year")->valueint;
                        moon_time = cJSON_GetObjectItem(cjson, "moon")->valueint;
                        sun_time = cJSON_GetObjectItem(cjson, "sun")->valueint;
                        hour_time = cJSON_GetObjectItem(cjson, "hour")->valueint;
                        minute_time = cJSON_GetObjectItem(cjson, "minute")->valueint;
                        second_time = cJSON_GetObjectItem(cjson, "second")->valueint;

                    }
                    time_init(year_time, moon_time, sun_time, hour_time, minute_time, second_time);
                    printf("time csjon解析成功\r\n");
                }





                //======================================================================================
                //
                //                value = 0;
                //                value = atoi(cJSON_GetObjectItem(cjson,"fan")->valuestring);
                //                if(value==1){
                ////                    Relay_Fan_On();
                //                    printf("风扇开启\r\n");
                ////                    LED_value = 1;
                //                }else if(value ==2){
                ////                    Relay_Fan_Off();
                ////                    LED_value = 0;
                //                    printf("风扇关闭\r\n");
                //                }
            }

            //delete cjson
            if(cjson != NULL)
            {
                cJSON_Delete(cjson);
                free(cjson);
                printf("释放成功\r\n");
            }

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
            if(dataPtr != NULL)
            {
                free(dataPtr);
                dataPtr = NULL;
                printf("释放dataptr空间成功\r\n");
            }
            if(req_payload != NULL)
            {
                free(req_payload);
                req_payload = NULL;
            }
            if(cmdid_topic != NULL)
            {
                free(cmdid_topic);
                cmdid_topic = NULL;
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

        //        //JSON字符串到cJSON格式
        //        cjson = cJSON_Parse(req_payload);
        //        //判断cJSON_Parse函数返回值确定是否打包成功
        //        if(cjson == NULL){
        ////						printf("json pack into cjson error...");
        //              printf( "json pack into cjson error...\r\n");
        //        }
        //        // 处理$crep的数据
        //        else{
        ////            value = 0;
        ////            value = atoi(cJSON_GetObjectItem(cjson,"fan")->valuestring);
        ////            if(value==1){
        ////                Relay_Fan_On();
        ////                printf("风扇开启\r\n");
        ////                LED_value = 1;
        ////            }else if(value ==2){
        ////                Relay_Fan_Off();
        ////                LED_value = 0;
        ////                printf("风扇关闭\r\n");
        ////            }
        //
        //
        //
        //
        //            //delete cjson
        //            cJSON_Delete(cjson);
        //
        //        }


        //=================$crep接收处理end=====================

    }

    if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
    {
        MQTT_FreeBuffer(cmdid_topic);
        MQTT_FreeBuffer(req_payload);
    }
    dataPtr -= pianyi;



}

