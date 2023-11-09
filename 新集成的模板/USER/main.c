#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "oled.h"
#include "usart.h"
#include "dht11.h"
#include "esp8266.h"
#include "AD.h"
#include "iwdg.h"


//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"
//c库
#include "string.h"

u16 value0;
u16 value1;
u16 value2;
u16 value3;
u16 value4;
u16 value5;

u8 buf1[10] = {0};
u8 buf2[10] = {0};
u8 buf3[10] = {0};
u8 buf4[10] = {0};
u8 buf5[10] = {0};
u8 buf6[10] = {0};
int i;

extern u8  flag;
int net_connect = 0;


int main(void)
{

    unsigned char *dataPtr = NULL;

    unsigned short timeCount = 0;	//发送间隔变量

    delay_init();	    	 //延时函数初始化
    NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    AD_Init();            //初始化ADC
    OLED_Init();			//初始化OLED
    OLED_Clear();
    relay_init();    //继电器
    LED_Init();
    uart_init(115200);//串口1初始化
    uart2_init(115200);//串口2初始化
    IWDG_Init(4, 625*5);	//溢出时间为5s  (4*2^4)*625/40=1000ms

    
    



    OLED_Clear();//清屏



    while(1)
    {
        
        if(net_connect == 1)
        {
            
//            for(i = 0, value0 = 0, value1 = 0, value2 = 0, value3 = 0, value4 = 0, value5 = 0; i < 60;)
//            {
//                value0 += AD_Value[i++];
//                value1 += AD_Value[i++];
//                value2 += AD_Value[i++];
//                value3 += AD_Value[i++];
//                value4 += AD_Value[i++];
//                value5 += AD_Value[i++];
//            }

//            sprintf((char *)buf1, "%d", value0 / 10);
//            sprintf((char *)buf2, "%d", value1 / 10);
//            sprintf((char *)buf3, "%d", value2 / 10);


//            sprintf((char *)buf4, "%d", value3 / 10);
//            sprintf((char *)buf5, "%d", value4 / 10);
//            sprintf((char *)buf6, "%d", value5 / 10);

//            OLED_ShowString(28, 0, buf1);
//            OLED_ShowString(28, 2, buf2);
//            OLED_ShowString(28, 4, buf3);

//            OLED_ShowString(95, 0, buf4);
//            OLED_ShowString(95, 2, buf5);
//            OLED_ShowString(95, 4, buf6);

//            delay_ms(100);

//            value0 = 0, value1 = 0, value2 = 0, value3 = 0, value4 = 0, value5 = 0;
//            
            

                OLED_ShowString(28, 0, "sdwd");
            if(++timeCount >= 100)		//时间间隔5s 发送
            {
                delay_ms(100);
                printf( "开始发送数据\r\n");//通过串口1发送提示信息（要开始发送数据了）
                OneNet_SendData();//发送数据给onenet
                printf("发送数据结束\n");
                timeCount = 0;
                ESP8266_Clear();
            }

            dataPtr = ESP8266_GetIPD(0);//获取平台返回的数据
            if(dataPtr != NULL)//如果返回数据不为空
                OneNet_RevPro(dataPtr);//平台返回数据检测

            delay_ms(10);
        }
        else
        {
            ESP8266_Init();					//初始化ESP8266

            printf("8266初始化成功\n");


            while(OneNet_DevLink())			//接入OneNET

            OLED_ShowCHinese(0, 3, 9); //接入云
            OLED_ShowCHinese(18, 3, 10);
            OLED_ShowCHinese(36, 3, 11);
            OLED_ShowCHinese(54, 3, 12);
            OLED_ShowCHinese(72, 3, 13);

            delay_ms(500);
            net_connect = 1;

            printf("接入onenet成功");
        }
        
        

    }

}
