#include "wifi.h"
#include "string.h"
#include "main.h"
#include "iwdg.h"
#define ESP8266_WIFI_INFO		"AT+CWJAP=\"me_computer\",\"2960493425\"\r\n"

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"
extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart2;

unsigned char rx_buff[MAX_SIZE];
unsigned short rx_cnt = 0;


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


_Bool ESP8266_SendCmd(char *cmd, char *res)
{

    unsigned char timeOut = 200;
    Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
    HAL_IWDG_Refresh(&hiwdg);
    while(timeOut--)
    {

        if(strstr((const char *)rx_buff, res) != NULL)		//����������ؼ���
        {
            ESP8266_Clear();									//��ջ���

            return 0;
        }


        HAL_Delay(10);
    }

    return 1;

}


void ESP8266_Init(void)
{
    ESP8266_Clear();
//    while(ESP8266_SendCmd("AT+RST\r\n", "ready"))
//        HAL_Delay(500);

    while(ESP8266_SendCmd("AT\r\n", "OK"))
        HAL_Delay(500);


    while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"))
        HAL_Delay(500);

    while(ESP8266_SendCmd("AT+CWDHCP=1,1\r\n", "OK"))
        HAL_Delay(500);



    while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "GOT IP"))
        HAL_Delay(500);

    while(ESP8266_SendCmd("AT+CIPMUX=0\r\n", "OK"))
        HAL_Delay(500);



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
        //        printf("\r\nprint = %c\r\n",*str++);
        HAL_Delay(1);
        //        while(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_TXE) == RESET){}
        //        USART_SendData(USARTx, *str++);									//��������
        //        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
    }

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
        Usart_SendString(USART2, data, len);		//�����豸������������
    }

}




