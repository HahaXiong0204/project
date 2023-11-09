#include "wifi.h"
#include "string.h"
#include "main.h"

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart3;



//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����
//==========================================================
void ESP8266_Clear(void)
{
    memset(rx_buff, 0, sizeof(rx_buff));
    rx_cnt = 0;

}

extern uint8_t net_connect;
extern uint8_t cmd_flag;    // �����ӡ��־λ
_Bool ESP8266_SendCmd(char *cmd, char *res)
{

    unsigned char timeOut = 200;
	if(net_connect == 0)
	{
		printf("����%s\r\n",cmd);
	}
	else if((net_connect==2|net_connect == 3)&&cmd_flag==0)
	{
		cmd_flag = 1;
		printf("�ȴ���������%s\r\n",cmd);
	}
	
    Usart_SendString(WIFI_USART, (unsigned char *)cmd, strlen((const char *)cmd));

    while(timeOut--)
    {
        if(strstr((const char *)rx_buff, res) != NULL)		//����������ؼ���
        {
            ESP8266_Clear();									//��ջ���
			if(cmd_flag){cmd_flag=0;}
            return 0;
        }

       osDelay(10);
    }
#if IWDG_FLAG
	HAL_IWDG_Refresh(&hiwdg);
#endif
    return 1;

}


void ESP8266_Init(void)
{
    ESP8266_Clear();
	
	Show_Str(10, 760, 200, 32, "�������� AT", 32, 0);
	while(ESP8266_SendCmd("AT\r\n", "OK"))
        HAL_Delay(100);
	
	Show_Str(10, 760, 200, 32, "�������� AT+RST", 32, 0);
    while(ESP8266_SendCmd("AT+RST\r\n", "ready"))
        HAL_Delay(2000);


	Show_Str(10, 760, 200, 32, "�������� AT+CWMODE=1", 32, 0);
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))    //����STAģʽ
        HAL_Delay(100);

	
	Show_Str(10, 760, 200, 32, "�������� AT+CWDHCP", 32, 0);
    while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
        HAL_Delay(100);


	Show_Str(10, 760, 200, 32, "�������� wifi", 32, 0);
    while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
        HAL_Delay(100);
	
    while(ESP8266_SendCmd("AT+CIPMUX=0\r\n", "OK"))
        HAL_Delay(100);


	Show_Str(10, 760, 200, 32, "�������ӷ�����", 32, 0);
    while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
        HAL_Delay(500);


    printf("7. ESP8266 Init OK\r\n");
}


//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//				��ESP8266�ķ��ظ�ʽΪ	"+IPD,x:yyy"	x�������ݳ��ȣ�yyy����������
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{
//#if IWDG_FLAG
//		HAL_IWDG_Refresh(&hiwdg);               // ι��
//#endif
    char *ptrIPD = NULL;

    do
    {
        ptrIPD = strstr((char *)rx_buff, "IPD,");				//������IPD��ͷ
        if(ptrIPD == NULL)											//���û�ҵ���������IPDͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
        {
//            printf("\"IPD\" not found\r\"");
        }
        else
        {
            ptrIPD = strchr(ptrIPD, ':');							//�ҵ�':'
            if(ptrIPD != NULL)
            {
                ptrIPD++;
                return (unsigned char *)(ptrIPD);
            }
            else
                return NULL;

        }


        HAL_Delay(5);													//��ʱ�ȴ�
    }
	
    while(timeOut--);

    return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

/*
************************************************************
*	�������ƣ�	Usart_SendString
*
*	�������ܣ�	�������ݷ���
*
*	��ڲ�����	USARTx��������
*				str��Ҫ���͵�����
*				len�����ݳ���
*
*	���ز�����	��
*
*	˵����
************************************************************
*/
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{

    unsigned short count = 0;
    //    printf("\r\n---------------------------\r\n");
    for(; count < len; count++)
    {
        USARTx->DR  = *str++;
        HAL_Delay(1);
    }
//#if IWDG_FLAG
//		HAL_IWDG_Refresh(&hiwdg);               // ι��
//#endif
}


//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����
//==========================================================
static uint8_t esp_num = 0; 
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

    char cmdBuf[32];

    ESP8266_Clear();								//��ս��ջ���
	
    //�ȷ���Ҫ�������ݵ�ָ����׼��
    sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//��������
    printf("cmdBuf=%s\r\n",cmdBuf);
    if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
    {
        //��Ȼ׼����ϼ��ɿ�ʼ��������
        printf("data = %s\r\n",data);
        Usart_SendString(WIFI_USART, data, len);		//�����豸������������
		esp_num = 0;
    }
	else{
		esp_num++;
		if(esp_num>3)
		{
			printf("wifi�Ͽ�����,׼������\r\n");
			ESP8266_Clear();									//��ջ���
			net_connect = 2;
			reconnect_num++;
			reconnect_one_num++;
			if(reconnect_one_num>3){
				NVIC_SystemReset();    // �����λ
			}
			printf("����net_connect =2ģʽ\r\n");
		
		
		}

	
	}

//#if IWDG_FLAG
//		HAL_IWDG_Refresh(&hiwdg);               // ι��
//#endif
}


