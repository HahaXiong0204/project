#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "dht11.h"
#include "oled.h"

//网络协议层
#include "onenet.h"
//网络设备
#include "esp8266.h"

#include "stdio.h"
#include "string.h"
#include "adc.h"

u8 threshold_temperature;    // 阈值温度
u8 threshold_shui;       // 土壤湿度
u8 temperature;            //温度
u8 fan;                    //风扇开关
u8 shui;
u8 humidity;               //湿度
u16 ADC_Value[40];         //adc数据缓冲区
u16 value1,value2,value3,value4;  //DMA数据处理的三个中间变量，分别提取光照，音量，烟雾的数据
u16 n=0;
float Distance;            //距离
u8 Lighting,Voice,Gas,Gas2;     //光照，音量，烟雾的数据
float temp1,temp2;
u8 LED_value;              //LED灯的值，0或1
u8 buf1[10]={0};
u8 buf2[10]={0};
u8 buf3[10]={0};
u8 buf4[10]={0};
u8 buf5[10]={0};
u8 buf6[10]={0};
u16 i;   //for循环用到的变量而已
u16 fire_value;



 int main(void)
{
	
	unsigned short timeCount = 0;	//发送间隔变量
	unsigned char *dataPtr = NULL;
    const char *topics[] = {"TWO_T"};
//	Hardware_Init();				//初始化外围硬件
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组
	delay_init();			          //延时函数初始化
	Usart1_Init(115200);							//串口1，打印信息用
	Usart2_Init(115200);
	OLED_Init(); 	              //OLED初始化  
	DHT11_Init();               //初始化温湿度模块
	ADCx_Init();                //初始化ADC，用于采集音量，光照，烟雾浓度
	Buzzer_Init();              //蜂鸣器初始化
	show();                     //OLED显示主界面
	ESP8266_Init();					//初始化ESP8266
	
	while(OneNet_DevLink())			//接入OneNET
		delay_ms(500);
	
	OneNet_Subscribe(topics, 1);
    threshold_shui = 20;
    threshold_temperature = 30;
//	

	while(1) 
	{
        
        
        
//=====================================温湿度模块===================================================	
        if(DHT11_Read_Data(&temperature,&humidity) == 0)
		{
			sprintf((char *)buf1,":%d",temperature);
			sprintf((char *)buf2,":%d%%RH",humidity);
			OLED_ShowStr(32,0,buf1,2);
			OLED_ShowStr(32,2,buf2,2);
		}
		
//=======================（光照，声音，有害气体）三个传感器数据的获取，通过ADC规则通道查询获取========		
        for(i=0,value1=0,value2=0,value3=0;i<40;)
		{
			value1+=ADC_Value[i++];	
			value2+=ADC_Value[i++];
			value3+=ADC_Value[i++];
            value4+=ADC_Value[i++];
		}	
//printf("val1 = %d   val2 = %d  val3= %d \r\n",value1,value2,value3);		
		Lighting=(4096-value1/10)*100/4096; //亮度
		temp1=(float)(value2/10)*(3.3/4096);//mq135
        temp2=(float)(value3/10)*(3.3/4096);  // mq2
        fire_value=(4096-value4/10)/41.0; //火焰值
//        printf("val1 = %d   val2 = %d  val3= %d val4 = %d\r\n",value1,value2,value3,value4);
//        printf("LIGHRT = %d    Tu = %d",Lighting,tu_si);

//		
		if(temp1<0.9)
        {
            n=temp1/0.001;
            Gas = n*0.012;
//				printf("一氧化碳浓度:%dppm\n",Gas);
        }
        else
        {
            Gas = ((temp1-0.9)/0.1)*100;
            Gas =Gas+9;
//				printf("一氧化碳浓度:%dppm\n",Gas);
        }
        
        
        if(temp2<0.9)
        {
            n=temp2/0.001;
            Gas2 = n*0.012;
//				printf("一氧化碳浓度:%dppm\n",Gas2);
        }
        else
        {
            Gas2 = ((temp2-0.9)/0.1)*100;
            Gas2 =Gas2+9;
//				printf("一氧化碳浓度:%dppm\n",Gas2);
        }
        
        
		sprintf((char *)buf1,":%d%%",Lighting);
        sprintf((char *)buf2,":%d%%",Gas);
		sprintf((char *)buf3,":%d%%",Gas2);
        sprintf((char *)buf4,":%d",fire_value);
//        printf("火焰：%d  \r\n",fire_value);
		OLED_ShowStr(32,4,buf1,2);//OLED显示亮度
        OLED_ShowStr(32,6,buf2,2);//OLED显示空气质量1
		OLED_ShowStr(96,6,buf3,2);//OLED显示空气质量2
        OLED_ShowStr(106,4,buf4,2);//OLED显示火焰
        
        value1=0;
		value2=0;
		value3=0;
        value4=0;
        

        
        
        
////===========================================================蜂鸣器报警===================================
////		if(tu_si<20)
////		{
////			Buzzer=Buzzer_ON;
////            LED = LED_ON;
////		}		
////        else
////		{
////			Buzzer=Buzzer_ON;
////            LED = LED_OFF;
////		}		
//		
        
        timeCount ++;
		if(timeCount >= 50)	//发送间隔3S
		{	
			timeCount = 0;
			OneNet_SendData();	//发送数据
			ESP8266_Clear();    //清空缓存
		}
		
//==============================================接受OneNet服务器发来的数据（可以不用管）===============================			
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
		{
            UsartPrintf(USART_DEBUG, "th_tem = %d\r\n",threshold_temperature);
            UsartPrintf(USART_DEBUG, "th_si = %d \r\n",threshold_shui);
			OneNet_RevPro(dataPtr);
		}
        
        

	
	
	}


}
















