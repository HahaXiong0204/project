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


#define PROID		"588384"   //��ƷID

#define AUTH_INFO	"123456"		//��Ȩ��Ϣ

#define DEVID		"1064133490"	//�豸ID





extern uint8_t mode;

extern double temperature;               //�¶�
extern double humidity;                  //ʪ��
extern uint32_t Lighting, Turbidity, Gas; //���գ��Ƕȣ����������
extern float voltage;  // ��ѹ


// �����ϴ���װ����  buf������  flag��1����ط�����  0:������ط�����
uint16_t id = 0 ;
uint16_t OneNet_FillBuf(char *buf, uint8_t again_flag)
{
    cJSON *send_cjson = NULL;
    char *str = (char *)malloc(1024);
    send_cjson = cJSON_CreateObject();   // ����cjson

    if(send_cjson != NULL)
    {
        printf("׼����װjson\r\n");
        //	cJSON_AddStringToObject(send_cjson,"name","MQ");		//����ַ���
        //	cJSON_AddNumberToObject(send_cjson,"age",25);	    	//�����������
        //	cJSON_AddNumberToObject(send_cjson,"height",183.52);	//��Ӹ���������
        //	cJSON_AddFalseToObject(send_cjson,"gender");			//����߼�ֵfalse

        if(again_flag)
            cJSON_AddNumberToObject(send_cjson, "Break_flag", 1); // �ط���־λ
		
		cJSON_AddNumberToObject(send_cjson, "Device_ID", DERVICE_ID);	    	//�����������
        cJSON_AddNumberToObject(send_cjson, "age", id++);	    	//�����������
		cJSON_AddNumberToObject(send_cjson, "ID_Cart", ID_Cart);			//���״ֵ̬
        cJSON_AddNumberToObject(send_cjson, "Temperature_clod", temperature);	//��Ӹ���������
		cJSON_AddNumberToObject(send_cjson, "Temperature_hot", Tc);	//��Ӹ���������
        cJSON_AddNumberToObject(send_cjson, "TDS1", Turbidity);	//��Ӹ���������
        cJSON_AddNumberToObject(send_cjson, "TDS2", voltage);	//��Ӹ���������
//        cJSON_AddNumberToObject(send_cjson, "Turbidity", Turbidity);	//��Ӹ���������
//        cJSON_AddNumberToObject(send_cjson, "voltage", voltage);	//��Ӹ���������

        str = cJSON_PrintUnformatted(send_cjson);
        printf("STR = %s\r\n", str);
        strcpy(buf, str);
        free(str);
        cJSON_Delete(send_cjson);
        return strlen(buf);
    }

    return 0;


}

void OneNet_Send_One_String_Data(char *key,char *value)
{
    cJSON *send_cjson = NULL;
    char *str = (char *)malloc(1024);
    send_cjson = cJSON_CreateObject();   // ����cjson
    if(send_cjson != NULL)
    {
        cJSON_AddStringToObject(send_cjson, key, value);			//���״ֵ̬
        str = cJSON_PrintUnformatted(send_cjson);
        printf("STR = %s\r\n", str);
        cJSON_Delete(send_cjson);		// �ͷ��ڴ�
        OneNet_SendData_Buff(str);
        free(str);

    }
}

void OneNet_Send_One_Int_Data(char *key,uint16_t value)
{
	 cJSON *send_cjson = NULL;
    char *str = (char *)malloc(1024);
    send_cjson = cJSON_CreateObject();   // ����cjson
    if(send_cjson != NULL)
    {
        cJSON_AddNumberToObject(send_cjson, key, value);			//���״ֵ̬
        str = cJSON_PrintUnformatted(send_cjson);
        printf("STR = %s\r\n", str);
        cJSON_Delete(send_cjson);		// �ͷ��ڴ�
        OneNet_SendData_Buff(str);
        free(str);

    }


}

void OneNet_Send_user_End(void)
{
	cJSON *send_cjson = NULL;
    char *str = (char *)malloc(1024);
    send_cjson = cJSON_CreateObject();   // ����cjson
    if(send_cjson != NULL)
    {
		/*
						year_time = cJSON_GetObjectItem(cjson, "year")->valueint;
                        moon_time = cJSON_GetObjectItem(cjson, "moon")->valueint;
                        sun_time = cJSON_GetObjectItem(cjson, "sun")->valueint;
                        hour_time = cJSON_GetObjectItem(cjson, "hour")->valueint;
                        minute_time = cJSON_GetObjectItem(cjson, "minute")->valueint;
                        second_time = cJSON_GetObjectItem(cjson, "second")->valueint;
		*/
		
		cJSON_AddNumberToObject(send_cjson, "ID_Cart", ID_Cart);			//���״ֵ̬
		cJSON_AddNumberToObject(send_cjson, "Device_ID", DERVICE_ID);	    	//�����������
		cJSON_AddNumberToObject(send_cjson, "water_intake", water_intake);
        cJSON_AddNumberToObject(send_cjson, "year_start", year_time_start);			//���״ֵ̬
		cJSON_AddNumberToObject(send_cjson, "moon_start", moon_time_start);			//���״ֵ̬
		cJSON_AddNumberToObject(send_cjson, "day_start", sun_time_start);			//���״ֵ̬
		cJSON_AddNumberToObject(send_cjson, "hour_start", hour_time_start);			//���״ֵ̬
		cJSON_AddNumberToObject(send_cjson, "minute_start", minute_time_start);			//���״ֵ̬
		cJSON_AddNumberToObject(send_cjson, "second_start", second_time_start);			//���״ֵ̬
		
		
		
        cJSON_AddNumberToObject(send_cjson, "year", year_time);			//���״ֵ̬
		cJSON_AddNumberToObject(send_cjson, "moon", moon_time);			//���״ֵ̬
		cJSON_AddNumberToObject(send_cjson, "day", sun_time);			//���״ֵ̬
		cJSON_AddNumberToObject(send_cjson, "hour", hour_time);			//���״ֵ̬
		cJSON_AddNumberToObject(send_cjson, "minute", minute_time);			//���״ֵ̬
		cJSON_AddNumberToObject(send_cjson, "second", second_time);			//���״ֵ̬
		
        str = cJSON_PrintUnformatted(send_cjson);
        printf("STR = %s\r\n", str);
        cJSON_Delete(send_cjson);		// �ͷ��ڴ�
        OneNet_SendData_Buff(str);
        free(str);

    }
}


// return 1 �ɹ�
// return 0 ʧ��
uint8_t OneNet_SendData_Buff(char *buf)
{
    MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���
    uint16_t body_len = strlen(buf);
    uint16_t i = 0;
    if(body_len)
    {
        if(MQTT_PacketSaveData(DEVID, body_len, NULL, 3, &mqttPacket) == 0)							//�����$dp
        {
            for(i = 0; i < body_len; i++)
            {
                mqttPacket._data[mqttPacket._len++] = buf[i];
            }
            ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//�ϴ����ݵ�ƽ̨
            printf( "Send %d Bytes\r\n", mqttPacket._len);

            MQTT_DeleteBuffer(&mqttPacket);															//ɾ��
            return 1;
        }
        else
            printf( "WARN:	EDP_NewBuffer Failed\r\n");

    }
    return 0;
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

    //	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//Э���

    char buf[256];

    //	short body_len = 0;

    printf( "Tips:	OneNet_SendData-MQTT\r\n");

    memset(buf, 0, sizeof(buf));

    OneNet_FillBuf(buf, 0);																	//��ȡ��ǰ��Ҫ���͵����������ܳ���

    OneNet_SendData_Buff(buf);

}



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
                case 0:
                    printf( "Tips:	���ӳɹ�\r\n");
                    status = 0;
                    break;

                case 1:
                    printf( "WARN:	����ʧ�ܣ�Э�����\r\n");
                    break;
                case 2:
                    printf( "WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");
                    break;
                case 3:
                    printf( "WARN:	����ʧ�ܣ�������ʧ��\r\n");
                    break;
                case 4:
                    printf( "WARN:	����ʧ�ܣ��û������������\r\n");
                    break;
                case 5:
                    printf( "WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");
                    break;

                default:
                    printf( "ERR:	����ʧ�ܣ�δ֪����\r\n");
                    break;
                }
            }
        }

        MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
        OneNet_Subscribe(topics, 1);   // ��������
    }
    else
        printf( "WARN:	MQTT_PacketConnect Failed\r\n");



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
        printf( "Subscribe Topic: %s\r\n", topics[i]);

    if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL2, topics, topic_cnt, &mqttPacket) == 0)
    {
        ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������

        MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
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


    //	int value;

    type = MQTT_UnPacketRecv(cmd);
    switch(type)
    {
    case MQTT_PKT_CMD:															//�����·�

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

    case MQTT_PKT_PUBLISH:														//���յ�Publish��Ϣ
        printf("��������MQTT_PKT_PUBLISH\r\n");
        result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
        if(result == 0)
        {
            printf( "topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
                    cmdid_topic, topic_len, req_payload, req_len);

            cJSON *cjson = NULL;

            //���Բ���
            double temp_value, humi_value;

            //JSON�ַ�����cJSON��ʽ
            cjson = cJSON_Parse(req_payload);
            //�ж�cJSON_Parse��������ֵȷ���Ƿ����ɹ�
            if(cjson == NULL)
            {
                //						printf("json pack into cjson error...");
                printf( "json pack into cjson error...\r\n");
            }
            // ����one������
            else
            {

                //��ȡ�ֶ�ֵ
                //cJSON_GetObjectltem���ص���һ��cJSON�ṹ���������ǿ���ͨ���������ؽṹ��ķ�ʽѡ�񷵻����ͣ�

                uint8_t flag = 0;
                printf("cjson �ڴ��СΪ = %d\r\n", sizeof(cjson));
                // ===========================================�������յ�������=====================================================

				
				if(cJSON_GetObjectItem(cjson, "temperature") != NULL)
                {
                    temp_value = cJSON_GetObjectItem(cjson, "temperature")->valuedouble;
                    printf("csjon�����ɹ� temp_value = %f\r\n", temp_value);
                }

                if(cJSON_GetObjectItem(cjson, "flag") != NULL)
                {
                    flag = cJSON_GetObjectItem(cjson, "flag")->valueint;
                    printf("csjon�����ɹ� flag = %d\r\n", flag);
                }

                if(cJSON_GetObjectItem(cjson, "Humidity") != NULL)
                {
                    humi_value = cJSON_GetObjectItem(cjson, "Humidity")->valuedouble;
                    printf("csjon�����ɹ� Humidity = %f\r\n", humi_value);
                }
				
				// ���ˮ��
				if(cJSON_GetObjectItem(cjson, "Th") != NULL)
                {
                    TH = cJSON_GetObjectItem(cjson, "Th")->valueint;
                    printf("csjon�����ɹ� TH = %d\r\n", TH);
                }

                /*
                	extern uint16_t year_time;
                	extern uint8_t moon_time;
                	extern uint8_t sun_time;
                	extern uint8_t hour_time;   // Сʱ
                	extern uint8_t minute_time;  // ����
                	extern uint8_t second_time;  // ��
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
					
					OneNet_Send_One_Int_Data("ID_Cart",ID_Cart);    // ���Ϳ���
				
					year_time_start = year_time;
					moon_time_start = moon_time;
					sun_time_start = sun_time;
					hour_time_start = hour_time; 
					minute_time_start = minute_time;
					second_time_start = second_time;
                    printf("time csjon�����ɹ�\r\n");
                }





                //======================================================================================
                //
                //                value = 0;
                //                value = atoi(cJSON_GetObjectItem(cjson,"fan")->valuestring);
                //                if(value==1){
                ////                    Relay_Fan_On();
                //                    printf("���ȿ���\r\n");
                ////                    LED_value = 1;
                //                }else if(value ==2){
                ////                    Relay_Fan_Off();
                ////                    LED_value = 0;
                //                    printf("���ȹر�\r\n");
                //                }
            }

            //delete cjson
            if(cjson != NULL)
            {
                cJSON_Delete(cjson);
                free(cjson);
                printf("�ͷųɹ�\r\n");
            }

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
            if(dataPtr != NULL)
            {
                free(dataPtr);
                dataPtr = NULL;
                printf("�ͷ�dataptr�ռ�ɹ�\r\n");
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

        //        //JSON�ַ�����cJSON��ʽ
        //        cjson = cJSON_Parse(req_payload);
        //        //�ж�cJSON_Parse��������ֵȷ���Ƿ����ɹ�
        //        if(cjson == NULL){
        ////						printf("json pack into cjson error...");
        //              printf( "json pack into cjson error...\r\n");
        //        }
        //        // ����$crep������
        //        else{
        ////            value = 0;
        ////            value = atoi(cJSON_GetObjectItem(cjson,"fan")->valuestring);
        ////            if(value==1){
        ////                Relay_Fan_On();
        ////                printf("���ȿ���\r\n");
        ////                LED_value = 1;
        ////            }else if(value ==2){
        ////                Relay_Fan_Off();
        ////                LED_value = 0;
        ////                printf("���ȹر�\r\n");
        ////            }
        //
        //
        //
        //
        //            //delete cjson
        //            cJSON_Delete(cjson);
        //
        //        }


        //=================$crep���մ���end=====================

    }

    if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
    {
        MQTT_FreeBuffer(cmdid_topic);
        MQTT_FreeBuffer(req_payload);
    }
    dataPtr -= pianyi;



}

