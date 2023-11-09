#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "oled.h"
#include "usart.h"
#include "dht11.h"
#include "esp8266.h"



//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"
//c库
#include "string.h"


	

//dht11添加变量
u8 humidityH;	  //湿度整数部分
u8 humidityL;	  //湿度小数部分
u8 temperatureH;   //温度整数部分
u8 temperatureL;   //温度小数部分
extern u8  flag;
//灯的状态
// u8 LED0_FLAG;
// u8 LED1_FLAG;
	
	DHT11_Data_TypeDef DHT11_Data;
 int main(void)
 {	

	unsigned char *dataPtr = NULL;
	 
	unsigned short timeCount = 0;	//发送间隔变量
	 
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
 	
	OLED_Init();			//初始化OLED   
	OLED_Clear();    
	LED_Init();			     //LED端口初始化
	 
	 
	uart_init(115200);//串口1初始化
	uart2_init(115200);//串口2初始化
	 
	DHT11_Init();//DHT11初始化
	 
	 
	ESP8266_Init();					//初始化ESP8266
	 
	 
	 
	printf("8266初始化成功\n");
	LED1=1;
	LED0=1;
	 
	while(OneNet_DevLink())			//接入OneNET
		 
		 	OLED_ShowCHinese(0,3,9);//接入云
			OLED_ShowCHinese(18,3,10);
			OLED_ShowCHinese(36,3,11);
			OLED_ShowCHinese(54,3,12);
			OLED_ShowCHinese(72,3,13);

	delay_ms(900);
	
	printf("接入onenet成功");
	
	  OLED_Clear();//清屏


	
	
	while(1) 
	{		
 	
		if(++timeCount >= 500)		//时间间隔5s
		{
				
			if( Read_DHT11(&DHT11_Data)==SUCCESS)										 //“\”表示转向一下行
			{
				//printf("temp %d hum %d",DHT11_Data.temp_int,DHT11_Data.humi_int);
				
				OLED_ShowCHinese(0,4,14);//温度
				OLED_ShowCHinese(18,4,15);
                OLED_ShowString(34,4,":"); 
				
				
				OLED_ShowNum(48,4,DHT11_Data.temp_int,2,16);
				OLED_ShowChar(70,4,'.');
				OLED_ShowNum(85,4,DHT11_Data.temp_deci,1,16);
				
				//printf("temp=%d.%d\n",DHT11_Data.temp_int,DHT11_Data.temp_deci);
				
				printf("hum=%d.%d\n",DHT11_Data.humi_int,DHT11_Data.humi_deci);
				
				
				OLED_ShowCHinese(0,2,16);//湿度
				OLED_ShowCHinese(18,2,15);
				OLED_ShowString(34,2,":"); 
				
				OLED_ShowNum(48,2,DHT11_Data.humi_int,2,16);
				OLED_ShowChar(70,2,'%'); 
				delay_ms(100);
			
				//主要用于数据上传使用
				humidityH=DHT11_Data.humi_int;	  //湿度整数部分
				humidityL=DHT11_Data.humi_deci;	  //湿度小数部分
				temperatureH=DHT11_Data.temp_int;   //温度整数部分
				temperatureL=DHT11_Data.temp_deci;   //温度小数部分
				printf("hum temp=%d .%d %d .%d\r\n",humidityH,humidityL,temperatureH,temperatureL);
				
				
			}
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
		
// 	  if(flag==1)
// 			LED0=1;
// 		else if(flag==2)
// 			LED0=0;
// 		else if(flag==3)
// 			LED1=1;
// 		else if(flag==4)
// 			LED1=0;
// 		
}

}
