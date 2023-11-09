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
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：
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

        if(strstr((const char *)rx_buff, res) != NULL)		//如果检索到关键词
        {
            ESP8266_Clear();									//清空缓存

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
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

    char *ptrIPD = NULL;

    do
    {
        ptrIPD = strstr((char *)rx_buff, "IPD,");				//搜索“IPD”头
        if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
        {
//            printf("\"IPD\" not found\r\"");
        }
        else
        {
            ptrIPD = strchr(ptrIPD, ':');							//找到':'
            if(ptrIPD != NULL)
            {
                ptrIPD++;
                return (unsigned char *)(ptrIPD);
            }
            else
                return NULL;

        }


        HAL_Delay(5);													//延时等待
    }
    while(timeOut--);

    return NULL;														//超时还未找到，返回空指针

}

/*
************************************************************
*	函数名称：	Usart_SendString
*
*	函数功能：	串口数据发送
*
*	入口参数：	USARTx：串口组
*				str：要发送的数据
*				len：数据长度
*
*	返回参数：	无
*
*	说明：
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
        //        USART_SendData(USARTx, *str++);									//发送数据
        //        while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
    }

}


//==========================================================
//	函数名称：	ESP8266_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

    char cmdBuf[32];

    ESP8266_Clear();								//清空接收缓存

    //先发送要发送数据的指令做准备
    sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
    printf("cmdBuf=%s\r\n",cmdBuf);
    if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
    {
        //既然准备完毕即可开始发送数据
        printf("data = %s\r\n",data);
        Usart_SendString(USART2, data, len);		//发送设备连接请求数据
    }

}




